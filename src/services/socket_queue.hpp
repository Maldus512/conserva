#ifndef SOCKETQ_H_INCLUDED
#define SOCKETQ_H_INCLUDED


#include <vector>
#include <optional>
#include <iostream>
#include <cstdint>
#include <filesystem>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

using std::optional;
using std::string;


class SocketQueueReceiver {
  public:
    explicit SocketQueueReceiver(std::filesystem::path path);
    ~SocketQueueReceiver(void);

    optional<string> receive(unsigned long timeout_ms);
    void             respond(string);

  private:
    std::filesystem::path path;
    int                   socket_fd;
    struct sockaddr_un    peer_socket;
    socklen_t             peer_socket_len;
};


class SocketQueueSender {
  public:
    SocketQueueSender(std::filesystem::path path);
    void             send(const string message);
    optional<string> receive_response(unsigned long timeout_ms);

  private:
    int                   socket_fd;
    std::filesystem::path path;
};


#endif
