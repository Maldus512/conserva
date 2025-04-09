# Conserva

A simple, down to earth pomodoro timer with a command line interface, a system tray and libnotify integration.

A [pomodoro](https://en.wikipedia.org/wiki/Pomodoro_Technique) time period is made of two time intervals: a work period and a relax period. `conserva` simply starts the timer and sends a notification on each state change.

Besides a system tray with options to start and stop the last timer, `conserva` offers a command line interface. Run `conserva --help` for more information.

## Genesis

This project was motivated by two main reasons:

 1. I needed a pomodoro timer that could easily be controller by command line or with a desktop environment keyboard shortcut.
 2. I wanted to learn two of the tools I despise most, CMake and C++.

I succeeded, although at great cost.

## TODO

 - Query state (also show in notification)
 - Print logs to log file (where?) (https://github.com/gabime/spdlog?tab=readme-ov-file)
 - Create apt packet
 - Create flatpak packet
 - Create snap packet
 - Save last activity to data file (where?)
 - Use a config file for images and sounds

## Credits

Icons created by [nangicon - Flaticon](https://www.flaticon.com/free-icons/pomodoro)
