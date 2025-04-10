#include "string.h"
#include <unistd.h>
#include <iostream>
#include "tray.hpp"
#include "tray.h"


#define ICON_STANDBY  BUILD_INSTALL_PREFIX "/share/conserva/icons/icon_standby.png"
#define ICON_WORKING  BUILD_INSTALL_PREFIX "/share/conserva/icons/icon_working.png"
#define ICON_RELAXING BUILD_INSTALL_PREFIX "/share/conserva/icons/icon_relaxing.png"


static void quit_cb(struct tray_menu_item *item);
static void start_cb(struct tray_menu_item *item);
static void stop_cb(struct tray_menu_item *item);
static void auto_reload_cb(struct tray_menu_item *item);
static void window_cb(struct tray *tray);


static struct tray tray = {
    .icon_filepath = ICON_STANDBY,
    .tooltip       = "Conserva",
    .cb            = window_cb,
    .menu =
        (struct tray_menu_item[]){
            {.text = "Start", .cb = start_cb},
            {.text = "Auto reload", .cb = auto_reload_cb},
            {.text = "Quit", .cb = quit_cb},
            {.text = NULL},
        },
};
static bool quit                = false;
static bool start_command       = false;
static bool stop_command        = false;
static bool report_command      = false;
static bool auto_reload_command = false;


Tray::Tray(void) {
    if (tray_init(&tray) < 0) {
        throw std::runtime_error("Failed to create tray!");
    }
}

bool Tray::manage(Model &model) {
    tray_loop(0);
    bool result = false;

    if (start_command) {
        result |= model.start_pomodoro();
        start_command = false;
    }
    if (stop_command) {
        result |= model.stop();
        stop_command = false;
    }
    if (report_command) {
        model.report   = true;
        report_command = false;
    }
    if (auto_reload_command) {
        model.auto_reload   = !model.auto_reload;
        auto_reload_command = false;
    }
    if (quit) {
        model.quit();
    }

    return result;
}

void Tray::update(Model &model) {
    switch (model.get_state()) {
        case Model::PomodoroState::standby: {
            tray.menu[0].text    = "Start";
            tray.menu[0].cb      = start_cb;
            tray.menu[0].checked = 0;
            tray.icon_filepath   = ICON_STANDBY;
            break;
        }
        case Model::PomodoroState::working: {
            tray.menu[0].text    = "Stop";
            tray.menu[0].cb      = stop_cb;
            tray.menu[0].checked = 1;
            tray.icon_filepath   = ICON_WORKING;
            break;
        }

        case Model::PomodoroState::relaxing: {
            tray.menu[0].text    = "Stop";
            tray.menu[0].cb      = stop_cb;
            tray.menu[0].checked = 1;
            tray.icon_filepath   = ICON_RELAXING;
            break;
        }
    }

    tray.menu[1].checked = model.auto_reload;

    tray_update(&tray);
}


Tray::~Tray(void) {
    tray_exit();
}


static void quit_cb(struct tray_menu_item *item) {
    (void)item;
    quit = true;
    tray_exit();
}


static void start_cb(struct tray_menu_item *item) {
    (void)item;
    start_command = true;
}


static void stop_cb(struct tray_menu_item *item) {
    (void)item;
    stop_command = true;
}


static void auto_reload_cb(struct tray_menu_item *item) {
    (void)item;
    auto_reload_command = true;
}


static void window_cb(struct tray *tray) {
    (void)tray;
    report_command = true;
}
