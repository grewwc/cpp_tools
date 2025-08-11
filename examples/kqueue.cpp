#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int kq = kqueue();
    assert(kq != -1);
    struct kevent event;
    int f = open("../temp.txt", O_RDONLY);
    EV_SET(&event, f, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_WRITE | NOTE_EXTEND | NOTE_DELETE, 0, nullptr);
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
        perror("kevent add");
        close(f);
        close(kq);
        return 1;
    }
    struct kevent event_out;

    while (true) {
        int ret = kevent(kq, nullptr, 0, &event_out, 1, nullptr);
        if (ret == -1) {
            printf("waiting events.");
        } else if (ret > 0) {
            if (event_out.ident == f) {
                // print("filter", event_out.filter);
                if (event_out.fflags & NOTE_WRITE) {
                    printf("file written");
                } else if (event_out.fflags & NOTE_EXTEND) {
                    printf("file size changed");
                } else if (event_out.fflags & NOTE_DELETE) {
                    printf("file deleted");
                }
            }
        }
    }
    close(kq);
    close(f);
}