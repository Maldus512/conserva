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


SocketQueueSender::SocketQueueSender(std::filesystem::path path) {
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd <= 0) {
        throw std::string("Client socket creation failed!");
    } else {
        this->socket_fd = fd;
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
    this->socket_fd = fd;
    this->path      = path;
}


SocketQueueReceiver::~SocketQueueReceiver(void) {
    std::filesystem::remove(this->path);
}


void SocketQueueSender::send(const std::string message) {
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, this->path.c_str());

    int res = sendto(this->socket_fd, message.data(), message.length(), 0, (struct sockaddr *)&remote, sizeof(remote));
    if (res != (int)message.length()) {
        std::cout << "Message send failed " << res << " \n";
    }
}

std::optional<std::string> SocketQueueReceiver::receive(uint32_t timeout_ms) {
    struct pollfd fds[1] = {{.fd = this->socket_fd, .events = POLLIN}};

    if (poll(fds, 1, timeout_ms)) {
        if (int length = 0; (length = recv(this->socket_fd, NULL, 0, MSG_PEEK | MSG_TRUNC)) > 0) {
            auto message = std::vector<char>();
            message.reserve(length);
            int res = recv(this->socket_fd, message.data(), length, 0);

            if (res == length) {
                auto string = std::string(message.data());
                return std::optional<std::string>(string);
            }
        }
    }

    return std::optional<std::string>();
}
