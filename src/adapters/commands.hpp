#ifndef COMMANDS_HPP_INCLUDED
#define COMMANDS_HPP_INCLUDED


#include <optional>
#include <iostream>
#include <stdint.h>
#include "services/socket_queue.hpp"
#include "model/model.hpp"


using std::optional;


namespace commands {

class Command {
  public:
    virtual ~Command(void)                            = default;
    virtual constexpr std::string toJson(void)        = 0;
    virtual bool                  visit(Model &model) = 0;     // Why.

    static optional<Command *> fromJson(std::string json_string);

  private:
};

class Start : public Command {
  public:
    Start(optional<std::string> name, optional<unsigned int> work_seconds, optional<unsigned int> relax_seconds)
        : name(name), work_seconds(work_seconds), relax_seconds(relax_seconds) {}

    std::string toJson(void) override;
    bool        visit(Model &model) override;

  private:
    optional<std::string>  name;
    optional<unsigned int> work_seconds;
    optional<unsigned int> relax_seconds;
};

class Stop : public Command {
  public:
    std::string toJson(void) override;
    bool        visit(Model &model) override;
};

class Server {
  public:
    static void remove_socket(void);

    Server(void);
    optional<Command *> receive(void);

  private:
    SocketQueueReceiver receiver;
};


void send(Command *command);

}     // namespace commands

#endif
