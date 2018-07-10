#ifndef _RBK_CORE_PUBLISHER_H_
#define _RBK_CORE_PUBLISHER_H_

#include <robokit/config.h>

#include <string>
#include <vector>
#include <functional>
#include <boost/thread/mutex.hpp>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/type_resolver_util.h>
#include <robokit/core/subscriber.h>
#include <robokit/core/thread_pool.h>

namespace rbk {
    namespace core {
        class Subscriber;
        class RBK_DLL_EXPORT BasePublisher {
        public:
            BasePublisher() {}
            virtual void close() {}
        protected:
            std::string m_topic;
        };

        class RBK_DLL_EXPORT Publisher : public BasePublisher {
        public:
            Publisher(std::string topic, std::string name) {
                m_topic = topic;
                m_func = NULL;
                m_initialized = false;
                m_name = name;
            };

            template<typename T>
            void publish(const T& data) {
                m_sub_mutex.lock();
                for (std::vector<Subscriber*>::iterator iter = m_subscribers.begin(); iter != m_subscribers.end(); iter++) {
                    rbk::ThreadPool::Instance()->schedule(boost::bind(&Subscriber::UpdateData<T>, *iter, data));
                    //(*iter)->UpdateData(data);
                }
                m_sub_mutex.unlock();

                m_func_mutex.lock();
                if (m_func != NULL) {
                    m_func(m_name, m_topic, data.SerializeAsString());
                }
                m_func_mutex.unlock();
            }

            //void publishInString(std::string data);

            template<typename T>
            void publish(const T& data, std::string recv_dev) {
                m_sub_mutex.lock();
                for (std::vector<Subscriber*>::iterator iter = m_subscribers.begin(); iter != m_subscribers.end(); iter++) {
                    if (recv_dev == (*iter)->name()) {
                        rbk::ThreadPool::Instance()->schedule(boost::bind(&Subscriber::UpdateData<T>, *iter, data));
                        //(*iter)->UpdateData(data);
                        break;
                    }
                }
                m_sub_mutex.unlock();
                m_func_mutex.lock();
                if (m_func != NULL) {
                    m_func(m_name, m_topic, data.SerializeAsString());
                }
                m_func_mutex.unlock();
            }

            bool addSubscriber(Subscriber* sub);
            bool addSubscriber(std::string, int, std::string, std::string);
            bool removeSubscriber(Subscriber* sub);
            bool removeSubscriber(std::string);

        private:
            bool m_initialized;
            std::vector<Subscriber*> m_subscribers;
            std::function<void(std::string&, std::string&, std::string)> m_func;
            boost::mutex m_sub_mutex;
            boost::mutex m_func_mutex;
            std::string m_name;
        };
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_PUBLISHER_H_
