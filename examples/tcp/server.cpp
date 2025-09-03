/**
 * single-threaded dummy server example
 *
**/ 

#include <iostream>

#include "NormalUtils.hpp"

// using namespace std;
using wwc::print;

#define PORT 7777
#define BUFFER_SIZE 1024

struct conn_t {
    int conn;
    sockaddr_in* client_addr;
};

void upper_case(char* buf, int len) {
    for (int i = 0; i < len; i++) {
        if (buf[i] >= 'a' && buf[i] <= 'z') {
            buf[i] += 'A' - 'a';
        }
    }
}

void handle_conn(conn_t con) {
    char buf[BUFFER_SIZE];
    print("conenction established", inet_ntoa(con.client_addr->sin_addr));
    memset(buf, 0, sizeof(buf));
    socklen_t socket_len = sizeof(*con.client_addr);

    size_t len = recv(con.conn, buf, sizeof(buf), 0);
    if (len > 0) {
        print("server received: ", buf);
        upper_case(buf, len);
        print("after modifying: ", buf);
        print("size:", len, "data", buf, inet_ntoa(con.client_addr->sin_addr), "len", len, "socket_len", socket_len);
        len = send(con.conn, buf, len, 0);
    } else {
        perror("here");
        exit(-1);
    }
}

int main() {
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    socklen_t socket_len = sizeof(client_addr);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("failed to open socket");
    }
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (int ret = ::bind(fd, (const sockaddr*)&server_addr, sizeof(server_addr)); ret < 0) {
        print(ret);
    } else {
        print("server is running");
    }

    if (listen(fd, 3) < 0) {
        perror("failed to listen");
        close(fd);
        return -1;
    }

    while (true) {
        int conn = accept(fd, (sockaddr*)&client_addr, &socket_len);
        if (conn < 0) {
            perror("failed to establish connection");
            close(fd);
            return -1;
        }

        handle_conn(conn_t{conn, &client_addr});
    }
    close(fd);
}
