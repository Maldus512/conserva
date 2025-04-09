#include "stopwatch.hpp"
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>

#define TIMESTAMP_AFTER(a, b)          ((long)((b) - (a)) < 0)
#define TIMESTAMP_AFTER_OR_EQUAL(a, b) (((long)((b) - (a)) <= 0))

static inline __attribute__((always_inline)) unsigned long timestamp_interval(unsigned long a, unsigned long b) {
    if (TIMESTAMP_AFTER(a, b))
        return -((unsigned long)b - (unsigned long)a);
    else
        return (unsigned long)b - (unsigned long)a;
}


static unsigned long timestamp_get(void);


bool Stopwatch::is_expired(void) const {
    return this->elapsed_milliseconds() > this->period_ms;
}


bool Stopwatch::is_running(void) const {
    return this->running;
}


bool Stopwatch::is_paused(void) const {
    return !this->running;
}


void Stopwatch::resume(void) {
    this->running   = true;
    this->timestamp = timestamp_get();
}

void Stopwatch::pause(void) {
    this->running    = false;
    this->elapsed_ms = timestamp_interval(timestamp_get(), this->timestamp);
}

unsigned long Stopwatch::remaining_milliseconds(void) const {
    if (this->is_expired()) {
        return 0;
    } else {
        return this->period_ms - this->elapsed_milliseconds();
    }
}

unsigned long Stopwatch::elapsed_milliseconds(void) const {
    if (this->running) {
        return this->elapsed_ms + timestamp_interval(timestamp_get(), this->timestamp);
    } else {
        return this->elapsed_ms;
    }
}

static unsigned long timestamp_get(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1000UL + ts.tv_nsec / 1000000UL;
}
