#include <iostream>
#include <iomanip>
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
    notify_notification_set_timeout(n, 8000);

    if (!notify_notification_show(n, 0)) {
        std::cerr << "show has failed" << std::endl;
    }
}


void Notifier::show_report(const Model &model) {
    auto oss = std::ostringstream();

    switch (model.get_state()) {
        case Model::PomodoroState::standby: {
            oss << "Standing by. Worked last on " << model.get_name() << ".";
            break;
        }

        case Model::PomodoroState::relaxing: {
            auto stopwatch         = model.get_stopwach();
            auto remaining_seconds = stopwatch.remaining_milliseconds() / 1000;
            oss << "Relax after " << model.get_name() << " for " << std::setfill('0') << std::setw(2)
                << remaining_seconds / 60 << ":" << std::setfill('0') << std::setw(2) << remaining_seconds % 60 << ".";
            break;
        }

        case Model::PomodoroState::working: {
            auto stopwatch         = model.get_stopwach();
            auto remaining_seconds = stopwatch.remaining_milliseconds() / 1000;
            oss << "Work on " << model.get_name() << " for " << std::setfill('0') << std::setw(2)
                << remaining_seconds / 60 << ":" << std::setfill('0') << std::setw(2) << remaining_seconds % 60 << ".";
            break;
        }
    }

    NotifyNotification *n = notify_notification_new("Conserva", oss.str().c_str(), 0);
    notify_notification_set_timeout(n, 4000);

    if (!notify_notification_show(n, 0)) {
        std::cerr << "show has failed" << std::endl;
    }
}
