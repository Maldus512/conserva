#ifndef CONTROLLER_HPP_INCLUDED
#define CONTROLLER_HPP_INCLUDED


#include <optional>
#include "model/model.hpp"
#include "adapters/notifier.hpp"
#include "adapters/tray.hpp"
#include "adapters/commands.hpp"


using std::optional;
using std::string;


class Controller {
  public:
    static void             send_start_message(optional<std::string> name, optional<unsigned int> work_seconds,
                                               optional<unsigned int> relax_seconds);
    static void             send_stop_message(void);
    static optional<string> send_report_message(void);
    static void             send_config_message(bool auto_reload);

    explicit Controller(bool auto_reload);
    void manage_server(void);
    bool should_quit(void) const;

  private:
    Model            model;
    Notifier         notifier;
    Tray             tray;
    commands::Server server;
};


#endif
