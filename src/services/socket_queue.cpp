#include <cstdint>
#include <optional>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>
#include "socket_queue.hpp"


static optional<string> receive_with_timeout(int socket_fd, struct sockaddr *sock_addr, socklen_t *peer_socket_len,
                                             unsigned long timeout_ms);


SocketQueueSender::SocketQueueSender(std::filesystem::path path) {
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd < 0) {
        throw string("Client socket creation failed!");
    } else {
        this->socket_fd = fd;
    }

    struct sockaddr_un client_addr;
    client_addr.sun_family  = AF_UNIX;
    client_addr.sun_path[0] = '\0';     // Abstract socket

    char client_socket_name[64] = {0};
    if (std::snprintf(client_socket_name, sizeof(client_socket_name), "conserva_client_socket_%d.sock", getpid()) >=
        (int)sizeof(client_socket_name)) {
        throw string("snprintf failed!");
    }
    strncpy(&client_addr.sun_path[1], client_socket_name, sizeof(client_addr.sun_path) - 2);

    if (bind(this->socket_fd, (struct sockaddr *)&client_addr, sizeof(sa_family_t) + 1 + strlen(client_socket_name))) {
        throw string("Bind failed!");
    }

    this->path = path;
}


SocketQueueReceiver::SocketQueueReceiver(std::filesystem::path path) {
    struct sockaddr_un local;
    int                len;
    int                fd;

    if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        throw std::filesystem::filesystem_error("Socket creation failed!", path, std::error_code());
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, path.c_str());
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(fd, (struct sockaddr *)&local, len) == -1) {
        std::cout << errno << "\n";
        throw std::filesystem::filesystem_error("Socket binding failed!", path, std::error_code());
    }

    this->socket_fd = fd;
    this->path      = path;
}


SocketQueueReceiver::~SocketQueueReceiver(void) {
    std::filesystem::remove(this->path);
}


void SocketQueueSender::send(const string message) {
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, this->path.c_str());

    int res = sendto(this->socket_fd, message.data(), message.length(), 0, (struct sockaddr *)&remote, sizeof(remote));
    if (res != (int)message.length()) {
        std::cout << "Message send failed " << res << " \n";
    }
}

optional<string> SocketQueueSender::receive_response(unsigned long timeout_ms) {
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, this->path.c_str());
    socklen_t peer_socket_len = sizeof(remote);

    return receive_with_timeout(this->socket_fd, (struct sockaddr *)&remote, &peer_socket_len, timeout_ms);
}

optional<string> SocketQueueReceiver::receive(unsigned long timeout_ms) {
    this->peer_socket_len = sizeof(this->peer_socket);
    auto res = receive_with_timeout(this->socket_fd, (struct sockaddr *)&this->peer_socket, &this->peer_socket_len,
                                    timeout_ms);

    return res;
}

void SocketQueueReceiver::respond(string message) {
    int res = sendto(this->socket_fd, message.data(), message.length(), 0, (struct sockaddr *)&this->peer_socket,
                     this->peer_socket_len);
    if (res != (int)message.length()) {
        std::cout << "Message send failed " << res << " " << strerror(errno) << "\n";
    }
}


static optional<string> receive_with_timeout(int socket_fd, struct sockaddr *sock_addr, socklen_t *peer_socket_len,
                                             unsigned long timeout_ms) {
    struct pollfd fds[1] = {{.fd = socket_fd, .events = POLLIN}};

    if (poll(fds, 1, timeout_ms)) {
        if (int length = 0; (length = recv(socket_fd, NULL, 0, MSG_PEEK | MSG_TRUNC)) > 0) {
            auto message = std::vector<char>();
            message.reserve(length);
            int res = recvfrom(socket_fd, message.data(), length, 0, sock_addr, peer_socket_len);

            if (res == length) {
                auto message_data = string(message.data(), res);
                return optional<string>(message_data);
            }
        }
    }

    return optional<string>();
}
