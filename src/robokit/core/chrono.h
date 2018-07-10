#ifndef _RBK_CHRONO_H_
#define _RBK_CHRONO_H_

#include <robokit/config.h>

#include <boost/chrono.hpp>

namespace rbk {
    namespace chrono {
        typedef boost::chrono::nanoseconds    nanoseconds;
        typedef boost::chrono::microseconds   microseconds;
        typedef boost::chrono::milliseconds   milliseconds;
        typedef boost::chrono::seconds        seconds;
        typedef boost::chrono::minutes        minutes;
        typedef boost::chrono::hours          hours;

        typedef boost::chrono::system_clock           systemClock;
        typedef boost::chrono::steady_clock           steadyClock;
        typedef boost::chrono::high_resolution_clock  highResolutionClock;

        typedef boost::chrono::time_point<boost::chrono::system_clock> systemClockTimePoint;
        typedef boost::chrono::time_point<boost::chrono::steady_clock> steadyClockTimePoint;
        typedef boost::chrono::time_point<boost::chrono::high_resolution_clock> highResolutionClockTimePoint;

        template <typename Clock>
        typename Clock::time_point now() {
            return typename Clock::now();
        }

        template <typename Duration, typename Rep, typename Period>
        Duration durationCast(const boost::chrono::duration<Rep, Period>& fd) {
            return boost::chrono::duration_cast<Duration>(fd);
        }
    } // namespace chrono
} // namespace rbk

#endif // ~_RBK_CHRONO_H_
