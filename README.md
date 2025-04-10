# Conserva 

con|sèr|va

pronunciation: */konˈsɛrva/*

*Feminine noun*

 1. **To preserve, especially foods (e.g. fruit or vegatables, tipically tomatoes).**
 2. Food prepared to be stored for long periods of time without losing its flavour and nutrition values.
 3. Container or place where something is preserved.

A simple, down to earth pomodoro timer with a command line interface, a system tray and libnotify integration.

A [pomodoro](https://en.wikipedia.org/wiki/Pomodoro_Technique) time period is made of two time intervals: a work period and a relax period. `conserva` simply starts the timer and sends a notification on each state change.

Besides a system tray with options to start and stop the last timer, `conserva` offers a command line interface. Run `conserva --help` for more information.

## Building

The project requires the following tools and dependencies:

 - `cmake`
 - `git`
 - `gtk+-3.0`
 - `libnotify`
 - `glib-2.0`
 - `gdk-pixbuf-2.0`
 - `jsoncpp`

To build, run:

```sh
$ cmake -G Ninja -B build/ -DCMAKE_INSTALL_PREFIX=<preferred installation path> -DCMAKE_BUILD_TYPE=Release
$ ninja -C build
```

To install at the specified installation path, run:

```sh
$ cmake --install build/
```

## Genesis

This project was motivated by two main reasons:

 1. I needed a pomodoro timer that could easily be controller by command line or with a desktop environment keyboard shortcut.
 2. I wanted to learn two tools I despise the most, CMake and C++.

It isn't pretty but it serves both purposes.

## TODO

 - [x] Query state (also show in notification)
 - [x] Add an "auto reload" option
 - [ ] Create snap packet
 - [x] Create apt packet
 - [ ] Create flatpak packet
 - [ ] Allow for customizable images and sounds in notification
 - [ ] Keep track of the last n activities and show them in a tray submenu

## Credits

Icons created by [nangicon - Flaticon](https://www.flaticon.com/free-icons/pomodoro)
