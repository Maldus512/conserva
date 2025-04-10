#ifndef COMMANDS_HPP_INCLUDED
#define COMMANDS_HPP_INCLUDED


#include <optional>
#include <iostream>
#include <stdint.h>
#include "services/socket_queue.hpp"
#include "model/model.hpp"


using std::optional;
using std::string;


namespace commands {

class Command {
  public:
    virtual ~Command(void)                                        = default;
    virtual string                            to_json(void) const = 0;
    virtual std::pair<bool, optional<string>> visit(Model &model) = 0;     // Why.
    virtual bool                              requires_response(void) const;

    static optional<Command *> from_json(string json_string);

  private:
};

class Start : public Command {
  public:
    Start(optional<string> name, optional<unsigned int> work_seconds, optional<unsigned int> relax_seconds)
        : name(name), work_seconds(work_seconds), relax_seconds(relax_seconds) {}

    string                            to_json(void) const override;
    std::pair<bool, optional<string>> visit(Model &model) override;

  private:
    optional<string>       name;
    optional<unsigned int> work_seconds;
    optional<unsigned int> relax_seconds;
};

class Stop : public Command {
  public:
    string                            to_json(void) const override;
    std::pair<bool, optional<string>> visit(Model &model) override;
};

class Report : public Command {
  public:
    string                            to_json(void) const override;
    std::pair<bool, optional<string>> visit(Model &model) override;
    bool                              requires_response(void) const override;
};

class Config : public Command {
  public:
    Config(bool auto_reload) : auto_reload(auto_reload){};

    string                            to_json(void) const override;
    std::pair<bool, optional<string>> visit(Model &model) override;

    bool auto_reload;

  private:
};

class Server {
  public:
    static void remove_socket(void);

    Server(void);
    bool receive(Model &model);

  private:
    SocketQueueReceiver receiver;
};


optional<string> send(const Command *command);

}     // namespace commands

#endif
