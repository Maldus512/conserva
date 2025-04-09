#include <sstream>
#include "notifier.hpp"
#include <libnotify/notify.h>

Notifier::Notifier(void) { 
    notify_init(BUILD_PROJECT_NAME);
}

void Notifier::show_update(const Model &model) {
    auto oss = std::ostringstream();

    switch (model.get_state()) {
        case Model::PomodoroState::standby: {
            oss << "Pomodoro " << model.get_name() << " done!";
            break;
        }

        case Model::PomodoroState::relaxing: {
            oss << "Work for " << model.get_name() << " done, relax a bit.";
            break;
        }

        case Model::PomodoroState::working: {
            oss << "Started work " << model.get_name() << ".";
            break;
        }
    }

    NotifyNotification *n = notify_notification_new("Conserva", oss.str().c_str(), 0);
    notify_notification_set_timeout(n, 8000);     // 10 seconds

    if (!notify_notification_show(n, 0)) {
        std::cerr << "show has failed" << std::endl;
    }
}
