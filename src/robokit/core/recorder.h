#ifndef _RBK_CORE_RECORDER_H_
#define _RBK_CORE_RECORDER_H_

#include <robokit/config.h>
#include <string>
#include <fstream>
#include <boost/thread/mutex.hpp>

namespace rbk {
    namespace core {
        class RBK_DLL_EXPORT Recorder {
        public:
            Recorder(std::string);
            ~Recorder();
            void recordLog(std::string, std::string);
            //void recordHeader(std::string);
        private:
            bool m_first_time;
            //	base::TimeCounter m_time_counter;
            std::ofstream m_fout;
            std::string m_file_name;
            boost::mutex m_mutex;
        };
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_RECORDER_H_
