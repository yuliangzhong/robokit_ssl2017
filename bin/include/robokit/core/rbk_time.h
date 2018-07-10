#ifndef _RBK_TIME_H_
#define _RBK_TIME_H_

//#ifdef _MSC_VER
//  // Rostime has some magic interface that doesn't directly include
//  // its implementation, this just disables those warnings.
//  #pragma warning(disable: 4244)
//  #pragma warning(disable: 4661)
//#endif
#include <robokit/config.h>
#include <iostream>
#include <cmath>
#include <boost/math/special_functions/round.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <robokit/core/duration.h>
/*********************************************************************
 ** Cross Platform Headers
 *********************************************************************/

#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

namespace boost {
    namespace posix_time {
        class ptime;
        class time_duration;
    }
}

namespace rbk
{
    namespace core {
        /*********************************************************************
        ** Functions
        *********************************************************************/

        RBK_DLL_EXPORT void normalizeSecNSec(uint64_t& sec, uint64_t& nsec);
        RBK_DLL_EXPORT void normalizeSecNSec(uint32_t& sec, uint32_t& nsec);
        RBK_DLL_EXPORT void normalizeSecNSecUnsigned(int64_t& sec, int64_t& nsec);

        /*********************************************************************
         ** Time Classes
         *********************************************************************/

         /**
          * \brief Base class for Time implementations.  Provides storage, common functions and operator overloads.
          * This should not need to be used directly.
          */
        template<class T, class D>
        class TimeBase
        {
        public:
            uint32_t sec, nsec;

            TimeBase() : sec(0), nsec(0) { }
            TimeBase(uint32_t _sec, uint32_t _nsec) : sec(_sec), nsec(_nsec)
            {
                normalizeSecNSec(sec, nsec);
            }
            explicit TimeBase(double t) { fromSec(t); }
            ~TimeBase() {}
            inline bool operator!=(const T &rhs) const { return !(*static_cast<const T*>(this) == rhs); }

            double toSec()  const { return (double)sec + 1e-9*(double)nsec; };
            T& fromSec(double t) { sec = (uint32_t)floor(t); nsec = (uint32_t)boost::math::round((t - sec) * 1e9);  return *static_cast<T*>(this); }

            uint64_t toNSec() const { return (uint64_t)sec * 1000000000ull + (uint64_t)nsec; }

            inline bool isZero() const { return sec == 0 && nsec == 0; }
            inline bool is_zero() const { return isZero(); }

            T& fromNSec(uint64_t t)
            {
                uint64_t sec64 = 0;
                uint64_t nsec64 = t;

                normalizeSecNSec(sec64, nsec64);

                sec = (uint32_t)sec64;
                nsec = (uint32_t)nsec64;

                return *static_cast<T*>(this);
            }

            D operator-(const T &rhs) const
            {
                return D((int32_t)sec - (int32_t)rhs.sec,
                    (int32_t)nsec - (int32_t)rhs.nsec); // carry handled in ctor
            }

            T operator-(const D &rhs) const
            {
                return *static_cast<const T*>(this) + (-rhs);
            }

            T operator+(const D &rhs) const
            {
                int64_t sec_sum = (int64_t)sec + (int64_t)rhs.sec;
                int64_t nsec_sum = (int64_t)nsec + (int64_t)rhs.nsec;

                // Throws an exception if we go out of 32-bit range
                normalizeSecNSecUnsigned(sec_sum, nsec_sum);

                // now, it's safe to downcast back to uint32 bits
                return T((uint32_t)sec_sum, (uint32_t)nsec_sum);
            }

            T& operator+=(const D &rhs)
            {
                *this = *this + rhs;
                return *static_cast<T*>(this);
            }

            T& operator-=(const D &rhs)
            {
                *this += (-rhs);
                return *static_cast<T*>(this);
            }

            bool operator==(const T &rhs) const
            {
                return sec == rhs.sec && nsec == rhs.nsec;
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

            boost::posix_time::ptime toBoost() const
            {
                namespace pt = boost::posix_time;
#if defined(BOOST_DATE_TIME_HAS_NANOSECONDS)
                return pt::from_time_t(sec) + pt::nanoseconds(nsec);
#else
                return pt::from_time_t(sec) + pt::microseconds(nsec / 1000.0);
#endif
            }
        };

        /**
         * \brief Time representation.  May either represent wall clock time or ROS clock time.
         *
         * ros::TimeBase provides most of its functionality.
         */
        class RBK_DLL_EXPORT Time : public TimeBase<Time, Duration>
        {
        public:
            Time()
                : TimeBase<Time, Duration>()
            {}

            Time(uint32_t _sec, uint32_t _nsec)
                : TimeBase<Time, Duration>(_sec, _nsec)
            {}

            explicit Time(double t) { fromSec(t); }

            /**
             * \brief Retrieve the current time.  If ROS clock time is in use, this returns the time according to the
             * ROS clock.  Otherwise returns the current wall clock time.
             */
            static Time now();
            /**
             * \brief Sleep until a specific time has been reached.
             */
            static bool sleepUntil(const Time& end);

            static void init();
            static void shutdown();
            static void setNow(const Time& new_now);
            static bool useSystemTime();
            static bool isSimTime();
            static bool isSystemTime();

            /**
             * \brief Returns whether or not the current time is valid.  Time is valid if it is non-zero.
             */
            static bool isValid();
            /**
             * \brief Wait for time to become valid
             */
            static bool waitForValid();
            /**
             * \brief Wait for time to become valid, with timeout
             */
            static bool waitForValid(const rbk::core::WallDuration& timeout);

            static Time fromBoost(const boost::posix_time::ptime& t);
            static Time fromBoost(const boost::posix_time::time_duration& d);
        };

        extern RBK_DLL_EXPORT const Time TIME_MAX;
        extern RBK_DLL_EXPORT const Time TIME_MIN;

        /**
         * \brief Time representation.  Always wall-clock time.
         *
         * ros::TimeBase provides most of its functionality.
         */
        class RBK_DLL_EXPORT WallTime : public TimeBase<WallTime, WallDuration>
        {
        public:
            WallTime()
                : TimeBase<WallTime, WallDuration>()
            {}

            WallTime(uint32_t _sec, uint32_t _nsec)
                : TimeBase<WallTime, WallDuration>(_sec, _nsec)
            {}

            explicit WallTime(double t) { fromSec(t); }

            /**
             * \brief Returns the current wall clock time.
             */
            static WallTime now();

            /**
             * \brief Sleep until a specific time has been reached.
             */
            static bool sleepUntil(const WallTime& end);

            static bool isSystemTime() { return true; }
        };

        std::ostream &operator <<(std::ostream &os, const Time &rhs);
        std::ostream &operator <<(std::ostream &os, const WallTime &rhs);
    }
}

#endif // _RBK_TIME_H_
