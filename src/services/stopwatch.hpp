#ifndef TIMESTAMP_H_INCLUDED
#define TIMESTAMP_H_INCLUDED


#include <stdint.h>


class Stopwatch {
  public:
    constexpr Stopwatch(unsigned long period_ms) : running(false), timestamp(0), elapsed_ms(0), period_ms(period_ms) {}

    bool          is_expired(void) const;
    bool          is_running(void) const;
    bool          is_paused(void) const;
    unsigned long remaining_milliseconds(void) const;
    unsigned long elapsed_milliseconds(void) const;
    void          resume(void);
    void          pause(void);

  private:
    bool          running;
    unsigned long timestamp;
    unsigned long elapsed_ms;
    unsigned long period_ms;
};


#endif
