#ifndef SOCKETQ_H_INCLUDED
#define SOCKETQ_H_INCLUDED


#include <vector>
#include <optional>
#include <iostream>
#include <cstdint>
#include <filesystem>


class SocketQueueReceiver {
  public:
    SocketQueueReceiver(std::filesystem::path path);
    ~SocketQueueReceiver(void);

    std::optional<std::string> receive(uint32_t timeout_ms);

  private:
    std::filesystem::path path;
    int                   socket_fd;
};


class SocketQueueSender {
  public:
    SocketQueueSender(std::filesystem::path path);
    void send(const std::string message);

  private:
    int                   socket_fd;
    std::filesystem::path path;
};


#endif
