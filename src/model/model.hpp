#ifndef MODEL_HPP_INCLUDED
#define MODEL_HPP_INCLUDED


#include <optional>
#include <iostream>
#include "services/stopwatch.hpp"


using std::optional;


class Model {
  public:
    enum class PomodoroState { standby, working, relaxing };
    struct PomodoroConfig {
        std::string   name;
        unsigned long work_seconds;
        unsigned long relax_seconds;

        constexpr bool operator==(PomodoroConfig config);
        constexpr bool operator!=(PomodoroConfig config);
    };

    explicit Model(void)
        : state(PomodoroState::standby), config({"work", 25 * 60, 5 * 60}), stopwatch(Stopwatch(0)), terminated(false) {
    }
    PomodoroState    get_state(void) const;
    std::string      get_name(void) const;
    bool             start_pomodoro(void);
    bool             start_pomodoro(optional<std::string> name, optional<unsigned int> work_seconds,
                                    optional<unsigned int> relax_seconds);
    bool             stop(void);
    bool             manage(void);
    void             quit(void);
    bool             is_terminated(void) const;
    const Stopwatch &get_stopwach(void) const;

  private:
    PomodoroState  state;
    PomodoroConfig config;
    Stopwatch      stopwatch;
    bool           terminated;
};


#endif
