#include <iostream>
#include <filesystem>
#include <unistd.h>
#include "services/socket_queue.hpp"
#include "config/app_config.h"
#include <argparse/argparse.hpp>
#include "signal.h"
#include "controller/controller.hpp"


static void                        signal_handler(int signal_num);
static std::optional<unsigned int> parse_time(std::string string);


int main(int argc, char *argv[]) {
    auto program = argparse::ArgumentParser(argv[0]);
    program.add_description("A command line first pomodoro timer")
        .add_epilog("Run with no arguments to start a server instance.");
    program.add_argument("-a", "--auto-reload").flag().help("Automatically reload pomodoro timer when finished");

    auto start = argparse::ArgumentParser("start");
    start.add_argument("-n", "--name").help("Pomodoro name");
    start.add_argument("-w", "--work-time").help("Work time; can be specified as seconds or minutes:seconds");
    start.add_argument("-r", "--relax-time").help("Relax time; can be specified as seconds or minutes:seconds");
    start.add_description("Start pomodoro iteration");

    auto stop = argparse::ArgumentParser("stop");
    stop.add_description("Pause current pomodoro");

    auto report = argparse::ArgumentParser("report");
    report.add_description("Print (and show in notification) current pomodoro information");

    auto config = argparse::ArgumentParser("config");
    config.add_description("Configure an existing instance");
    config.add_argument("-a", "--auto-reload").flag().help("Automatically reload pomodoro timer when finished");

    program.add_subparser(start);
    program.add_subparser(stop);
    program.add_subparser(report);
    program.add_subparser(config);

    try {
        program.parse_args(argc, argv);

        if (program["--help"] == true) {
            std::cout << program << std::endl;
        }

        // Start message
        if (program.is_subcommand_used(start)) {
            auto work_time    = start.present("--work-time");
            auto work_seconds = std::optional<unsigned int>();

            if (work_time.has_value()) {
                work_seconds = parse_time(work_time.value());
                if (!work_seconds.has_value()) {
                    std::cerr << "Invalid time input: " << work_time.value();
                    std::cerr << program << std::endl;
                    return 1;
                }
            }

            auto relax_time    = start.present("--relax-time");
            auto relax_seconds = std::optional<unsigned int>();
            if (relax_time.has_value()) {
                relax_seconds = parse_time(relax_time.value());
                if (!relax_seconds.has_value()) {
                    std::cerr << "Invalid time input: " << relax_time.value();
                    std::cerr << program << std::endl;
                    return 1;
                }
            }

            Controller::send_start_message(start.present("--name"), work_seconds, relax_seconds);
        }
        // Stop message
        else if (program.is_subcommand_used(stop)) {
            Controller::send_stop_message();
        }
        // Config message
        else if (program.is_subcommand_used(config)) {
            Controller::send_config_message(program["--auto-reload"] == true);
        }
        // Report message
        else if (program.is_subcommand_used(report)) {
            if (auto response = optional<string>(); (response = Controller::send_report_message()).has_value()) {
                std::cout << response.value() << "\n";
            }
        }
        // Server mode
        else {
            Controller controller = Controller(program["--auto-reload"] == true);

            {
                struct sigaction sa;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags   = 0;
                sa.sa_handler = signal_handler;
                sigaction(SIGINT, &sa, nullptr);
            }

            while (!controller.should_quit()) {
                controller.manage_server();
            }
        }
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    return 0;
}


static void signal_handler(int signal_num) {
    commands::Server::remove_socket();
    exit(1);
}


static std::optional<unsigned int> parse_time(std::string string) {
    unsigned int minutes = 0;
    unsigned int seconds = 0;

    if (std::sscanf(string.c_str(), "%i:%i", &minutes, &seconds) == 2) {
        return std::optional<unsigned int>(minutes * 60 + seconds);
    } else if (std::sscanf(string.c_str(), "%i", &seconds) == 1) {
        return std::optional<unsigned int>(seconds);
    } else {
        return std::optional<unsigned int>();
    }
}
