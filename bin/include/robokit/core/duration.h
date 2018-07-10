#ifndef _RBK_DURATION_H_
#define _RBK_DURATION_H_

#include <robokit/config.h>

#include <iostream>
#include <math.h>
#include <stdexcept>
#include <climits>
#include <stdint.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace boost {
    namespace posix_time {
        class time_duration;
    }
}

namespace rbk {
    namespace core {
        RBK_DLL_EXPORT void normalizeSecNSecSigned(int64_t& sec, int64_t& nsec);
        RBK_DLL_EXPORT void normalizeSecNSecSigned(int32_t& sec, int32_t& nsec);

        /**
         * \brief Base class for Duration implementations.  Provides storage, common functions and operator overloads.
         * This should not need to be used directly.
         */
        template <class T>
        class DurationBase
        {
        public:
            int32_t sec, nsec;
            DurationBase() : sec(0), nsec(0) { }
            DurationBase(int32_t _sec, int32_t _nsec)
                : sec(_sec), nsec(_nsec) {
                normalizeSecNSecSigned(sec, nsec);
            }
            explicit DurationBase(double t) { fromSec(t); };
            ~DurationBase() {}

            T operator+(const T &rhs) const {
                return T(sec + rhs.sec, nsec + rhs.nsec);
            }

            T operator*(double scale) const {
                return T(toSec() * scale);
            }

            T operator-(const T &rhs) const {
                return T(sec - rhs.sec, nsec - rhs.nsec);
            }

            T operator-() const {
                return T(-sec, -nsec);
            }

            T& operator+=(const T &rhs) {
                *this = *this + rhs;
                return *static_cast<T*>(this);
            }

            T& operator-=(const T &rhs) {
                *this += (-rhs);
                return *static_cast<T*>(this);
            }

            T& operator*=(double scale)
            {
                fromSec(toSec() * scale);
                return *static_cast<T*>(this);
            }

            bool operator<(const T &rhs) const
            {
                if (sec < rhs.sec)
                    return true;
                else if (sec == rhs.sec && nsec < rhs.nsec)
                    return true;
                return false;
            }

            bool operator>(const T &rhs) const
            {
                if (sec > rhs.sec)
                    return true;
                else if (sec == rhs.sec && nsec > rhs.nsec)
                    return true;
                return false;
            }

            bool operator<=(const T &rhs) const
            {
                if (sec < rhs.sec)
                    return true;
                else if (sec == rhs.sec && nsec <= rhs.nsec)
                    return true;
                return false;
            }

            bool operator>=(const T &rhs) const
            {
                if (sec > rhs.sec)
                    return true;
                else if (sec == rhs.sec && nsec >= rhs.nsec)
                    return true;
                return false;
            }

            bool operator==(const T &rhs) const
            {
                return sec == rhs.sec && nsec == rhs.nsec;
            }

            inline bool operator!=(const T &rhs) const { return !(*static_cast<const T*>(this) == rhs); }
            double toSec() const { return (double)sec + 1e-9*(double)nsec; };
            int64_t toNSec() const { return (int64_t)sec * 1000000000ll + (int64_t)nsec; };

            T& fromSec(double d) {
                sec = (int32_t)floor(d);
                nsec = (int32_t)((d - (double)sec) * 1000000000);
                return *static_cast<T*>(this);
            }

            T& fromNSec(int64_t t) {
                sec = (int32_t)(t / 1000000000);
                nsec = (int32_t)(t % 1000000000);

                normalizeSecNSecSigned(sec, nsec);
                return *static_cast<T*>(this);
            }

            bool isZero() const {
                return sec == 0 && nsec == 0;
            }

            boost::posix_time::time_duration toBoost() const {
                namespace bt = boost::posix_time;
#if defined(BOOST_DATE_TIME_HAS_NANOSECONDS)
                return bt::seconds(sec) + bt::nanoseconds(nsec);
#else
                return bt::seconds(sec) + bt::microseconds(nsec / 1000.0);
#endif
            }
        };

        class Rate;

        /**
         * \brief Duration representation for use with the Time class.
         *
         * ros::DurationBase provides most of its functionality.
         */
        class RBK_DLL_EXPORT Duration : public DurationBase<Duration>
        {
        public:
            Duration()
                : DurationBase<Duration>()
            { }

            Duration(int32_t _sec, int32_t _nsec)
                : DurationBase<Duration>(_sec, _nsec)
            {}

            explicit Duration(double t) { fromSec(t); }
            explicit Duration(const Rate&);
            /**
             * \brief sleep for the amount of time specified by this Duration.  If a signal interrupts the sleep, resleeps for the time remaining.
             */
            bool sleep() const;
        };

        extern const Duration DURATION_MAX;
        extern const Duration DURATION_MIN;

        /**
         * \brief Duration representation for use with the WallTime class.
         *
         * ros::DurationBase provides most of its functionality.
         */
        class RBK_DLL_EXPORT WallDuration : public DurationBase<WallDuration>
        {
        public:
            WallDuration()
                : DurationBase<WallDuration>()
            { }

            WallDuration(int32_t _sec, int32_t _nsec)
                : DurationBase<WallDuration>(_sec, _nsec)
            {}

            explicit WallDuration(double t) { fromSec(t); }
            explicit WallDuration(const Rate&);
            /**
             * \brief sleep for the amount of time specified by this Duration.  If a signal interrupts the sleep, resleeps for the time remaining.
             */
            bool sleep() const;
        };

        std::ostream &operator <<(std::ostream &os, const Duration &rhs);
        std::ostream &operator <<(std::ostream &os, const WallDuration &rhs);
    }
}

#endif // ~_RBK_DURATION_H_
