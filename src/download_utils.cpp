#include "download_utils.hpp"

size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    auto& mem = *static_cast<std::string*>(userp);
    mem.append(static_cast<char*>(contents), realsize);
    return realsize;
}

static std::once_flag init_flag;
static std::once_flag cleanup_flag;
namespace wwc {
    bool download_url(const char* url, std::string& chunk) {
        CURL* curl_handle;
        CURLcode res;

        std::call_once(init_flag, curl_global_init, CURL_GLOBAL_ALL);
        std::call_once(init_flag, curl_global_init, CURL_GLOBAL_ALL);
        curl_global_init(CURL_GLOBAL_ALL);

        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        // added options that may be required
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);   // redirects
        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10L);  // redirects
        curl_easy_setopt(curl_handle, CURLOPT_HTTPPROXYTUNNEL, 1L);  // corp. proxies etc.
        curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);          // we want it all
        curl_easy_setopt(curl_handle, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

        res = curl_easy_perform(curl_handle);
        bool ok = true;
        if (res != CURLE_OK) {
            ok = false;
        }

        curl_easy_cleanup(curl_handle);
        std::call_once(cleanup_flag, curl_global_cleanup);
        return ok;
    }
}  // namespace wwc