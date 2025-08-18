#include <curl/curl.h>
#include <fcntl.h>
#include <sys/event.h>
#include <unistd.h>

#include <iostream>
#include <unordered_map>
#include <vector>

class AsyncDownloader {
private:
    CURLM* multi_handle;
    int kq;
    int wakeup_pipe[2];  // 用于唤醒 kqueue
    std::unordered_map<curl_socket_t, struct kevent> socket_events;

public:
    AsyncDownloader() {
        multi_handle = curl_multi_init();

        // 创建 kqueue
        kq = kqueue();
        if (kq == -1) {
            throw std::runtime_error("Failed to create kqueue");
        }

        // 创建唤醒管道
        if (pipe(wakeup_pipe) == -1) {
            throw std::runtime_error("Failed to create pipe");
        }
        fcntl(wakeup_pipe[0], F_SETFL, O_NONBLOCK);

        // 注册管道到 kqueue
        struct kevent change;
        EV_SET(&change, wakeup_pipe[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        kevent(kq, &change, 1, nullptr, 0, nullptr);

        // 设置 curl socket 回调
        curl_multi_setopt(multi_handle, CURLMOPT_SOCKETFUNCTION, socket_callback);
        curl_multi_setopt(multi_handle, CURLMOPT_SOCKETDATA, this);

        // 设置 timer 回调
        curl_multi_setopt(multi_handle, CURLMOPT_TIMERFUNCTION, timer_callback);
        curl_multi_setopt(multi_handle, CURLMOPT_TIMERDATA, this);
    }

    ~AsyncDownloader() {
        curl_multi_cleanup(multi_handle);
        close(wakeup_pipe[0]);
        close(wakeup_pipe[1]);
        close(kq);
    }

    // 添加下载任务
    void addDownload(const std::string& url, const std::string& filename) {
        CURL* curl = curl_easy_init();
        if (curl) {
            // 设置下载选项
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

            // 创建文件信息
            FILE* file = fopen(filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            // 将文件指针存储在 curl 的私有数据中
            curl_easy_setopt(curl, CURLOPT_PRIVATE, file);

            // 添加到 multi handle
            curl_multi_add_handle(multi_handle, curl);
        }
    }

    // 事件循环
    void run() {
        while (true) {
            // 检查是否有活动的传输
            int still_running;
            curl_multi_perform(multi_handle, &still_running);

            if (still_running == 0) {
                std::cout << "All downloads completed" << std::endl;
                break;
            }

            // 等待事件
            struct kevent events[64];
            int nev = kevent(kq, nullptr, 0, events, 64, nullptr);

            if (nev > 0) {
                for (int i = 0; i < nev; i++) {
                    if (events[i].ident == wakeup_pipe[0]) {
                        // 处理唤醒事件
                        char buffer[1];
                        while (read(wakeup_pipe[0], buffer, 1) > 0) {
                        }
                    } else {
                        // 处理 socket 事件
                        curl_socket_t sockfd = events[i].ident;
                        int ev_bitmask = 0;

                        if (events[i].filter == EVFILT_READ) {
                            ev_bitmask |= CURL_CSELECT_IN;
                        }
                        if (events[i].filter == EVFILT_WRITE) {
                            ev_bitmask |= CURL_CSELECT_OUT;
                        }

                        int running_handles;
                        curl_multi_socket_action(multi_handle, sockfd, ev_bitmask, &running_handles);
                    }
                }

                // 检查完成的传输
                check_multi_info();
            }
        }
    }

private:
    // 写入回调
    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        FILE* file = (FILE*)userp;
        if (file) {
            return fwrite(contents, size, nmemb, file);
        }
        return 0;
    }

    // Socket 回调
    static int socket_callback(CURL* easy, curl_socket_t s, int what, void* userp, void* socketp) {
        AsyncDownloader* downloader = static_cast<AsyncDownloader*>(userp);
        return downloader->handle_socket(easy, s, what);
    }

    int handle_socket(CURL* easy, curl_socket_t s, int what) {
        struct kevent change;
        int events = 0;

        switch (what) {
            case CURL_POLL_IN: events = EVFILT_READ; break;
            case CURL_POLL_OUT: events = EVFILT_WRITE; break;
            case CURL_POLL_INOUT: events = EVFILT_READ | EVFILT_WRITE; break;
            case CURL_POLL_REMOVE:
                // 移除事件监听
                EV_SET(&change, s, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
                kevent(kq, &change, 1, nullptr, 0, nullptr);
                EV_SET(&change, s, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                kevent(kq, &change, 1, nullptr, 0, nullptr);
                socket_events.erase(s);
                return 0;
        }

        // 添加或修改事件监听
        if (events & EVFILT_READ) {
            EV_SET(&change, s, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
            kevent(kq, &change, 1, nullptr, 0, nullptr);
        }
        if (events & EVFILT_WRITE) {
            EV_SET(&change, s, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
            kevent(kq, &change, 1, nullptr, 0, nullptr);
        }

        socket_events[s] = change;
        return 0;
    }

    // Timer 回调
    static int timer_callback(CURLM* multi, long timeout_ms, void* userp) {
        AsyncDownloader* downloader = static_cast<AsyncDownloader*>(userp);
        downloader->handle_timer(timeout_ms);
        return 0;
    }

    void handle_timer(long timeout_ms) {
        // 简单实现：唤醒 kqueue 进行检查
        write(wakeup_pipe[1], "t", 1);
    }

    // 检查完成的传输
    void check_multi_info() {
        CURLMsg* message;
        int pending;

        while ((message = curl_multi_info_read(multi_handle, &pending))) {
            switch (message->msg) {
                case CURLMSG_DONE: {
                    CURL* easy = message->easy_handle;
                    CURLcode result = message->data.result;

                    if (result == CURLE_OK) {
                        std::cout << "Download completed successfully" << std::endl;
                    } else {
                        std::cout << "Download failed: " << curl_easy_strerror(result) << std::endl;
                    }

                    // 关闭文件
                    FILE* file;
                    curl_easy_getinfo(easy, CURLINFO_PRIVATE, &file);
                    if (file) {
                        fclose(file);
                    }

                    // 清理
                    curl_multi_remove_handle(multi_handle, easy);
                    curl_easy_cleanup(easy);
                    break;
                }
                default: break;
            }
        }
    }
};

int main() {
    try {
        AsyncDownloader downloader;

        // 添加下载任务
        downloader.addDownload("https://httpbin.org/image/jpeg", "image1.jpg");
        downloader.addDownload("https://httpbin.org/image/png", "image2.png");

        std::cout << "Starting downloads..." << std::endl;
        downloader.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
