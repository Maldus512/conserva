#include "string.h"
#include <unistd.h>
#include <iostream>
#include "tray.hpp"
#include "tray.h"

#if defined(_WIN32) || defined(_WIN64)
#define TRAY_WINAPI 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define TRAY_QT 1
#elif defined(__APPLE__) || defined(__MACH__)
#define TRAY_APPKIT 1
#endif

#if TRAY_QT
#define TRAY_ICON1 "assets/icon-24px.png"
#define TRAY_ICON2 "assets/icon2-24px.png"
#elif TRAY_APPKIT
#define TRAY_ICON1 "assets/icon.png"
#define TRAY_ICON2 "assets/icon2.png"
#elif TRAY_WINAPI
#define TRAY_ICON1 "assets/icon.ico"
#define TRAY_ICON2 "assets/icon2.ico"
#endif


static void quit_cb(struct tray_menu_item *item);
static void start_cb(struct tray_menu_item *item);
static void stop_cb(struct tray_menu_item *item);


void window_cb(struct tray *tray) {
    (void)tray;
}

void toggle_cb(struct tray_menu_item *item) {
    printf("toggle cb\n");
    item->checked     = !item->checked;
    struct tray *tray = tray_get_instance();
    if (tray != NULL)
        tray_update(tray);
}

void hello_cb(struct tray_menu_item *item) {
    (void)item;
    printf("hello cb: changing icon\n");
    struct tray *tray = tray_get_instance();
    if (tray == NULL)
        return;
    if (strcmp(tray->icon_filepath, TRAY_ICON1) == 0) {
        tray->icon_filepath = TRAY_ICON2;
    } else {
        tray->icon_filepath = TRAY_ICON1;
    }
    tray_update(tray);
}

void submenu_cb(struct tray_menu_item *item) {
    (void)item;
    printf("submenu: clicked on %s\n", item->text);
    //  tray_update(tray_get_instance());
}

static struct tray tray = {
    .icon_filepath = TRAY_ICON1,
    .tooltip       = "Conserva",
    .cb            = window_cb,
    .menu =
        (struct tray_menu_item[]){
            {.text = "Start", .cb = start_cb},
            {.text = "Quit", .cb = quit_cb},
            {.text = NULL},
        },
};
static bool quit          = false;
static bool start_command = false;
static bool stop_command  = false;


Tray::Tray(void) {
    if (tray_init(&tray) < 0) {
        throw std::runtime_error("failed to create tray!");
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
            tray.menu[0].checked = 1;
            break;
        }
        case Model::PomodoroState::working:
        case Model::PomodoroState::relaxing: {
            tray.menu[0].text    = "Stop";
            tray.menu[0].cb      = stop_cb;
            tray.menu[0].checked = 0;
            break;
        }
    }
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
