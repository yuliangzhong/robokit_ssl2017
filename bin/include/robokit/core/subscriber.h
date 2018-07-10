#ifndef _RBK_CORE_SUBSCRIBER_H_
#define _RBK_CORE_SUBSCRIBER_H_

#include <robokit/config.h>

#include <boost/thread/mutex.hpp>
#include <functional>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
//#include <robokit/core/recorder.h>

namespace rbk {
    namespace core {
        class RBK_DLL_EXPORT BaseSubscriber {
        public:
            BaseSubscriber() {}
            virtual void close() {}
            std::string topic() { return m_topic; }
        protected:
            std::string m_topic;
        };

        class Recorder;
        class RBK_DLL_EXPORT Subscriber : public BaseSubscriber {
        public:
            Subscriber(std::string topic, std::string name);
            ~Subscriber();

            template <typename T, typename CallBack>
            void init(CallBack globalFunc) {
                if (m_initialized) {
                    return;
                }

                m_msg = new T;
                m_tmp_msg = new T;
                if (globalFunc != NULL) {
                    m_func = globalFunc;
                }
                m_initialized = true;
            }

            template <typename T, typename CallBack, typename T1>
            void init(CallBack memberFunc, T1 *pThis) {
                if (m_initialized) {
                    return;
                }

                m_msg = new T;
                m_tmp_msg = new T;
                m_func = std::bind(memberFunc, pThis, std::placeholders::_1);
                m_initialized = true;
            }

            template <typename T>
            void init() {
                if (m_initialized) {
                    return;
                }

                m_msg = new T;
                m_tmp_msg = new T;
                m_func = NULL;
                m_initialized = true;
            }

            void init(std::function<void(google::protobuf::Message*)> globalFunc);

            template<typename T>
            void UpdateData(T data) {
                m_mutex.lock();
                static bool first_in = true;
                if (first_in && m_msg == NULL) {
                    first_in = false;
                    m_msg = new T;
                    m_tmp_msg = new T;
                }
                m_msg->CopyFrom(data);
                m_mutex.unlock();
                if (NULL != m_func) {
                    m_func(&data);
                }

                //if (m_log_recorder) {
                //	m_log_recorder->recordLog(m_topic, data.SerializeAsString());
                //}
            }

            template<typename T>
            void GetData(T& data) {
                m_mutex.lock();
                data.CopyFrom(*m_msg);
                m_mutex.unlock();
            }

            std::string GetDataInString();
            void UpdateDataInString(std::string data);
            std::string name();
            //void RegisterLogRecorder(Recorder* l);
        private:
            bool m_initialized;
            bool m_need_record_log;
            std::string m_name;
            std::function<void(google::protobuf::Message*)> m_func;
            google::protobuf::Message* m_msg;
            google::protobuf::Message* m_tmp_msg;
            boost::mutex m_mutex;
            //Recorder* m_log_recorder;
        };
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_SUBSCRIBER_H_
