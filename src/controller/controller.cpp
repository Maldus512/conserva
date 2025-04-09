#include "controller.hpp"
#include "model/model.hpp"
#include "config/app_config.h"
#include <unistd.h>


Controller::Controller(void) : model(Model()), notifier(Notifier()), tray(Tray()), server(commands::Server()) {}


void Controller::manage_server(void) {
    bool update = false;

    update |= this->tray.manage(this->model);

    if (auto command = optional<commands::Command *>(); (command = this->server.receive()).has_value()) {
        update |= command.value()->visit(model);
    }

    update |= this->model.manage();

    if (update) {
        this->tray.update(model);
        this->notifier.show_update(model);
    }
}

bool Controller::should_quit(void) const {
    return this->model.is_terminated();
}

void Controller::send_start_message(optional<std::string> name, optional<unsigned int> work_seconds,
                                    optional<unsigned int> relax_seconds) {
    commands::send(new commands::Start(name, work_seconds, relax_seconds));
}


void Controller::send_stop_message(void) {
    commands::send(new commands::Stop());
}
