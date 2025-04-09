#include "services/stopwatch.hpp"
#include "model.hpp"

using PomodoroState  = Model::PomodoroState;
using PomodoroConfig = Model::PomodoroConfig;




PomodoroState Model::get_state(void) const {
    return this->state;
}


std::string Model::get_name(void) const {
    return this->config.name;
}


bool Model::start_pomodoro(void) {
    return this->start_pomodoro(optional<std::string>(), optional<unsigned int>(), optional<unsigned int>());
}


bool Model::start_pomodoro(optional<std::string> name, optional<unsigned int> work_seconds,
                           optional<unsigned int> relax_seconds) {
    auto config = this->config;

    if (name.has_value()) {
        config.name = name.value();
    }
    if (work_seconds.has_value()) {
        config.work_seconds = work_seconds.value();
    }
    if (relax_seconds.has_value()) {
        config.relax_seconds = relax_seconds.value();
    } else if (work_seconds.has_value()) {
        config.relax_seconds = work_seconds.value() / 5;
    }

    if (this->state != PomodoroState::working || this->config != config) {
        this->state  = PomodoroState::working;
        this->config = config;

        this->stopwatch = Stopwatch(config.work_seconds * 1000UL);
        this->stopwatch.resume();

        return true;
    } else {
        return false;
    }
}


bool Model::stop(void) {
    if (this->state != PomodoroState::standby) {
        this->state = PomodoroState::standby;
        this->stopwatch.pause();
        return true;
    } else {
        return false;
    }
}


const Stopwatch &Model::get_stopwach(void) const {
    return this->stopwatch;
}


bool Model::is_terminated(void) const {
    return this->terminated;
}


void Model::quit(void) {
    this->terminated = true;
}


bool Model::manage(void) {
    switch (this->state) {
        case PomodoroState::standby: {
            break;
        }
        case PomodoroState::working: {
            if (this->stopwatch.is_running() && this->stopwatch.is_expired()) {
                this->state     = PomodoroState::relaxing;
                this->stopwatch = Stopwatch(config.relax_seconds * 1000UL);
                this->stopwatch.resume();
                return true;
            }
            break;
        }
        case PomodoroState::relaxing: {
            if (this->stopwatch.is_running() && this->stopwatch.is_expired()) {
                this->state = PomodoroState::standby;
                this->stopwatch.pause();
                return true;
            }
            break;
        }
    }

    return false;
}

constexpr bool PomodoroConfig::operator==(const PomodoroConfig config) {
    return this->relax_seconds == config.relax_seconds && this->work_seconds == config.work_seconds &&
           this->name == config.name;
}

constexpr bool PomodoroConfig::operator!=(const PomodoroConfig config) {
    return !(*this == config);
}
