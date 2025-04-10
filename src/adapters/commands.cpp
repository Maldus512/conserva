#include <optional>
#include "json/json.h"
#include "commands.hpp"
#include "config/app_config.h"
#include "services/socket_queue.hpp"


template <typename... Args> std::string string_format(const std::string &format, Args... args);


static std::string simple_command_to_json(std::string key);


static const auto json_key_name          = "name";
static const auto json_key_work_seconds  = "workSeconds";
static const auto json_key_relax_seconds = "relaxSeconds";
static const auto json_key_start         = "start";
static const auto json_key_stop          = "stop";
static const auto json_key_report        = "report";
static const auto json_key_config        = "config";
static const auto json_key_auto_reload   = "autoReload";


namespace commands {

bool Command::requires_response(void) const {
    return false;
}


bool Report::requires_response(void) const {
    return true;
}


std::string Start::to_json(void) const {
    Json::Value root;
    Json::Value data = Json::objectValue;

    if (this->name.has_value()) {
        data[json_key_name] = Json::String(this->name.value());
    } else {
        data[json_key_name] = Json::nullValue;
    }
    if (this->work_seconds.has_value()) {
        data[json_key_work_seconds] = Json::Int(this->work_seconds.value());
    } else {
        data[json_key_work_seconds] = Json::nullValue;
    }
    if (this->relax_seconds.has_value()) {
        data[json_key_relax_seconds] = Json::Int(this->relax_seconds.value());
    } else {
        data[json_key_relax_seconds] = Json::nullValue;
    }
    root[json_key_start] = data;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}


std::pair<bool, optional<std::string>> Start::visit(Model &model) {
    return std::pair(model.start_pomodoro(this->name, this->work_seconds, this->relax_seconds),
                     optional<std::string>());
}

std::pair<bool, optional<std::string>> Stop::visit(Model &model) {
    return std::pair(model.stop(), optional<std::string>());
}


std::pair<bool, optional<std::string>> Config::visit(Model &model) {
    if (model.auto_reload != this->auto_reload) {
        model.auto_reload = this->auto_reload;
        return std::pair(true, optional<std::string>());
    } else {
        return std::pair(false, optional<std::string>());
    }
}


std::pair<bool, optional<std::string>> Report::visit(Model &model) {
    model.report = true;

    auto oss = std::ostringstream();

    switch (model.get_state()) {
        case Model::PomodoroState::standby: {
            oss << "standby";
            break;
        }

        case Model::PomodoroState::relaxing: {
            auto stopwatch         = model.get_stopwach();
            auto remaining_seconds = stopwatch.remaining_milliseconds() / 1000;
            oss << "relax," << model.get_name() << "," << std::setfill('0') << std::setw(2) << remaining_seconds / 60
                << ":" << std::setfill('0') << std::setw(2) << remaining_seconds % 60;
            break;
        }

        case Model::PomodoroState::working: {
            auto stopwatch         = model.get_stopwach();
            auto remaining_seconds = stopwatch.remaining_milliseconds() / 1000;
            oss << "work," << model.get_name() << "," << std::setfill('0') << std::setw(2) << remaining_seconds / 60
                << ":" << std::setfill('0') << std::setw(2) << remaining_seconds % 60;
            break;
        }
    }

    return std::pair(false, optional(oss.str()));
}


std::string Stop::to_json(void) const {
    return simple_command_to_json(json_key_stop);
}


std::string Report::to_json(void) const {
    return simple_command_to_json(json_key_report);
}


std::string Config::to_json(void) const {
    Json::Value root;
    Json::Value data = Json::objectValue;

    data[json_key_auto_reload] = this->auto_reload;
    root[json_key_config]      = data;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}


optional<Command *> Command::from_json(std::string json_string) {
    const auto                              length = static_cast<int>(json_string.length());
    Json::CharReaderBuilder                 builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    JSONCPP_STRING err;
    Json::Value    root;

    if (!reader->parse(json_string.c_str(), json_string.c_str() + length, &root, &err)) {
        // error!
    } else if (root.isObject()) {

        auto json_start  = root[json_key_start];
        auto json_stop   = root[json_key_stop];
        auto json_report = root[json_key_report];
        auto json_config = root[json_key_config];

        // Start message
        if (json_start.isObject()) {
            auto json_name          = json_start[json_key_name];
            auto json_work_seconds  = json_start[json_key_work_seconds];
            auto json_relax_seconds = json_start[json_key_relax_seconds];

            auto name         = json_name == Json::nullValue ? optional<std::string>() : optional(json_name.asString());
            auto work_seconds = json_work_seconds == Json::nullValue
                                    ? optional<unsigned int>()
                                    : optional<unsigned int>(json_work_seconds.asInt());
            auto relax_seconds = json_relax_seconds == Json::nullValue
                                     ? optional<unsigned int>()
                                     : optional<unsigned int>(json_relax_seconds.asInt());

            return optional(new Start(name, work_seconds, relax_seconds));
        }
        // Stop message
        else if (json_stop.isObject()) {
            return optional(new Stop());
        }
        // Config message
        else if (json_config.isObject()) {
            auto json_auto_reload = json_config[json_key_auto_reload];
            if (json_auto_reload.isBool()) {
                return optional(new Config(json_auto_reload.asBool()));
            } else {
                return optional<Command *>();
            }
        }
        // Report message
        else if (json_report.isObject()) {
            return optional(new Report());
        }
    }

    return optional<Command *>();
}

void Server::remove_socket(void) {
    std::filesystem::remove(string_format(APP_CONFIG_SOCKET_PATH_FMT, getpid()));
}

Server::Server(void) : receiver(string_format(APP_CONFIG_SOCKET_PATH_FMT, getpid())) {}

bool Server::receive(Model &model) {
    if (auto message = optional<std::string>(); (message = this->receiver.receive(10)).has_value()) {
        if (auto command = optional<Command *>(); (command = Command::from_json(message.value())).has_value()) {
            auto result = command.value()->visit(model);
            delete command.value();

            if (result.second.has_value()) {
                this->receiver.respond(result.second.value());
            }

            return result.first;
        }
    };

    return false;
}


optional<string> send(const Command *command) {
    std::string path = "/tmp";
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (int pid = 0; std::sscanf(entry.path().filename().c_str(), APP_CONFIG_SOCKET_NAME_FMT, &pid) == 1) {
            SocketQueueSender socketQueueSender = SocketQueueSender(entry.path());
            socketQueueSender.send(command->to_json());
            if (command->requires_response()) {
                return socketQueueSender.receive_response(200);
            }
        }
    }

    return optional<string>();
}

}     // namespace commands


template <typename... Args> std::string string_format(const std::string &format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;     // Extra space for '\0'
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting!");
    }

    auto size = static_cast<size_t>(size_s);
    auto buf  = std::unique_ptr<char[]>(new char[size]);

    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);     // We don't want the '\0' inside
}


static std::string simple_command_to_json(std::string key) {
    Json::Value root;
    Json::Value data = Json::objectValue;

    root[key] = data;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}
