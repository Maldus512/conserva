#include <optional>
#include "json/json.h"
#include "commands.hpp"
#include "config/app_config.h"
#include "services/socket_queue.hpp"


template <typename... Args> std::string string_format(const std::string &format, Args... args);


static const auto json_key_name          = "name";
static const auto json_key_work_seconds  = "workSeconds";
static const auto json_key_relax_seconds = "relaxSeconds";
static const auto json_key_start         = "start";
static const auto json_key_stop          = "stop";


namespace commands {

std::string Start::toJson(void) {
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


bool Start::visit(Model &model) {
    return model.start_pomodoro(this->name, this->work_seconds, this->relax_seconds);
}

bool Stop::visit(Model &model) {
    return model.stop();
}


std::string Stop::toJson(void) {
    Json::Value root;
    Json::Value data = Json::objectValue;

    root[json_key_stop] = data;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}


optional<Command *> Command::fromJson(std::string json_string) {
    const auto                              length = static_cast<int>(json_string.length());
    Json::CharReaderBuilder                 builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    JSONCPP_STRING err;
    Json::Value    root;

    if (!reader->parse(json_string.c_str(), json_string.c_str() + length, &root, &err)) {
        // error!
    } else if (root.isObject()) {

        auto json_start = root[json_key_start];
        auto json_stop  = root[json_key_stop];

        // Start message
        if (json_start.isObject()) {
            auto json_name          = json_start[json_key_name];
            auto json_work_seconds  = json_start[json_key_work_seconds];
            auto json_relax_seconds = json_start[json_key_relax_seconds];

            auto name =
                json_name == Json::nullValue ? optional<std::string>() : optional<std::string>(json_name.asString());
            auto work_seconds  = json_work_seconds == Json::nullValue
                                     ? optional<unsigned int>()
                                     : optional<unsigned int>(json_work_seconds.asInt());
            auto relax_seconds = json_relax_seconds == Json::nullValue
                                     ? optional<unsigned int>()
                                     : optional<unsigned int>(json_relax_seconds.asInt());

            return optional<Command *>(new Start(name, work_seconds, relax_seconds));
        }
        // Stop message
        else if (json_stop.isObject()) {
            return optional<Command *>(new Stop());
        }
    }

    return optional<Command *>();
}

void Server::remove_socket(void) {
    std::filesystem::remove(string_format(APP_CONFIG_SOCKET_PATH_FMT, getpid()));
}

Server::Server(void) : receiver(string_format(APP_CONFIG_SOCKET_PATH_FMT, getpid())) {}

optional<Command *> Server::receive(void) {
    if (auto message = optional<std::string>(); (message = this->receiver.receive(10)).has_value()) {
        if (auto command = optional<Command *>(); (command = Command::fromJson(message.value())).has_value()) {
            return optional<Command *>(command);
        }
    };

    return optional<Command *>();
}


void send(Command *command) {
    std::string path = "/tmp";
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (int pid = 0; std::sscanf(entry.path().filename().c_str(), APP_CONFIG_SOCKET_NAME_FMT, &pid) == 1) {
            SocketQueueSender socketQueueSender = SocketQueueSender(entry.path());
            socketQueueSender.send(command->toJson());
        }
    }
}

}     // namespace commands


template <typename... Args> std::string string_format(const std::string &format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;     // Extra space for '\0'
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }

    auto size = static_cast<size_t>(size_s);
    auto buf  = std::unique_ptr<char[]>(new char[size]);

    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);     // We don't want the '\0' inside
}
