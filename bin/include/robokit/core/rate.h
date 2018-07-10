#ifndef _RBK_RATE_H_
#define _RBK_RATE_H_

#include <robokit/config.h>
#include <robokit/core/rbk_time.h>

namespace rbk
{
    namespace core {
        class Duration;

        /**
         * @class Rate
         * @brief Class to help run loops at a desired frequency
         */
        class RBK_DLL_EXPORT Rate
        {
        public:
            /**
             * @brief  Constructor, creates a Rate
             * @param  frequency The desired rate to run at in Hz
             */
            Rate(double frequency);
            explicit Rate(const Duration&);

            /**
             * @brief  Sleeps for any leftover time in a cycle. Calculated from the last time sleep, reset, or the constructor was called.
             * @return True if the desired rate was met for the cycle, false otherwise.
             */
            bool sleep();

            /**
             * @brief  Sets the start time for the rate to now
             */
            void reset();

            /**
             * @brief  Get the actual run time of a cycle from start to sleep
             * @return The runtime of the cycle
             */
            Duration cycleTime() const;

            /**
             * @brief Get the expected cycle time -- one over the frequency passed in to the constructor
             */
            Duration expectedCycleTime() const { return expected_cycle_time_; }

        private:
            Time start_;
            Duration expected_cycle_time_, actual_cycle_time_;
        };

        /**
         * @class WallRate
         * @brief Class to help run loops at a desired frequency.  This version always uses wall-clock time.
         */
        class RBK_DLL_EXPORT WallRate
        {
        public:
            /**
             * @brief  Constructor, creates a Rate
             * @param  frequency The desired rate to run at in Hz
             */
            WallRate(double frequency);
            explicit WallRate(const Duration&);

            /**
             * @brief  Sleeps for any leftover time in a cycle. Calculated from the last time sleep, reset, or the constructor was called.
             * @return Passes through the return value from WallDuration::sleep() if it slept, otherwise True
             */
            bool sleep();

            /**
             * @brief  Sets the start time for the rate to now
             */
            void reset();

            /**
             * @brief  Get the actual run time of a cycle from start to sleep
             * @return The runtime of the cycle
             */
            WallDuration cycleTime() const;

            /**
             * @brief Get the expected cycle time -- one over the frequency passed in to the constructor
             */
            WallDuration expectedCycleTime() const { return expected_cycle_time_; }

        private:
            WallTime start_;
            WallDuration expected_cycle_time_, actual_cycle_time_;
        };
    }
}

#endif // ~_RBK_RATE_H_
