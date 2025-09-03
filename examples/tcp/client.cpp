#include "NormalUtils.hpp"

using namespace std;

using wwc::print;
#define PORT 7777
#define IP = "127.0.0.1"

int main(int argc, char* argv[]) {
    sockaddr_in server_addr;
    socklen_t socket_len = sizeof(server_addr);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("failed to create socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;

    const char* msg = "hello world";
    if (argc > 1) {
        msg = argv[1];
    }
    if (connect(fd, (const sockaddr*)&server_addr, socket_len) < 0) {
        perror("failed to connect to the server");
        close(fd);
        return -1;
    }
    int len = send(fd, msg, strlen(msg), 0);
    print("send msg to server.", len);

    sockaddr client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    char buf[1024] = {0};
    len = recv(fd, buf, sizeof(buf), 0);
    print("received from server: ", buf, "len", len);
    close(fd);
}
