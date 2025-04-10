#include "controller.hpp"
#include "model/model.hpp"
#include "config/app_config.h"
#include <unistd.h>


static optional<string> send_command(const commands::Command *command);


Controller::Controller(bool auto_reload)
    : model(Model(auto_reload)), notifier(Notifier()), tray(Tray()), server(commands::Server()) {
        this->tray.update(model);
    }


void Controller::manage_server(void) {
    bool update = false;

    update |= this->tray.manage(this->model);
    update |= this->server.receive(this->model);
    update |= this->model.manage();

    if (model.report) {
        this->notifier.show_report(model);
        model.report = false;
    } else if (update) {
        this->tray.update(model);
        this->notifier.show_update(model);
    }
}

bool Controller::should_quit(void) const {
    return this->model.is_terminated();
}

void Controller::send_start_message(optional<std::string> name, optional<unsigned int> work_seconds,
                                    optional<unsigned int> relax_seconds) {
    send_command(new commands::Start(name, work_seconds, relax_seconds));
}


void Controller::send_stop_message(void) {
    send_command(new commands::Stop());
}


void Controller::send_config_message(bool auto_reload) {
    send_command(new commands::Config(auto_reload));
}


optional<string> Controller::send_report_message(void) {
    return send_command(new commands::Report());
}


optional<string> send_command(const commands::Command *command) {
    auto result = commands::send(command);
    delete command;
    return result;
}
