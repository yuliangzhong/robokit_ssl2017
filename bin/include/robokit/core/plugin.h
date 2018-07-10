#ifndef _RBK_CORE_PLUGIN_H_
#define _RBK_CORE_PLUGIN_H_

#include <string>
#include <vector>
#include <map>

#include <robokit/config.h>
#include <robokit/core/publisher.h>
#include <robokit/core/subscriber.h>
#include <robokit/core/service.h>
#include <robokit/core/attribute.h>
#include <robokit/core/event.h>
#include <robokit/core/delegate.h>
#include <robokit/core/lua_reader.h>
#include <robokit/core/logger.h>
//#include <robokit/core/recorder.h>
#include <robokit/core/sleep.h>
#include <robokit/core/param.h>

#if RBK_USE_CPP11
#include <robokit/core/luawapper.h>
#endif

namespace rbk {
    namespace core {
        class RBK_API NPlugin {
        private:
            NPlugin(const NPlugin&);
            NPlugin& operator=(const NPlugin&);
        protected:
            std::string m_device_name;
            std::string m_original_device_name;

            std::map<std::string, Publisher*> m_publishers;
            std::map<std::string, Subscriber*> m_subscribers;
            std::map<std::string, BaseService*> m_services;
            std::map<std::string, BaseAttribute*> m_attributes;
            std::map<std::string, Event*> m_events;
            std::map<std::string, Delegate*> m_delegates;
            Params _params;

            //Recorder* m_log_recorder;
            lua_State* m_lua_state;

        public:
            NPlugin();
            NPlugin(const std::string& name);
            virtual void loadFromConfigFile() = 0;
            virtual void setSubscriberCallBack() = 0;
            virtual void run() = 0;
            void start();
            void initialize();
            std::string name() const;
            std::string originalName() const;
            void setOriginalName(const std::string& name);
            void setName(const std::string& name);
            void setLuaState(lua_State* s);

            // void registerLogRecorder();
            // void registerLogRecorder(const std::string& topic);
            Subscriber* getSubscriber(const std::string& topic) const;
            Publisher* getPublisher(const std::string& topic);
            void connectTo(const NPlugin* recv_dev, const std::string& topic);
            void disconnectTo(const NPlugin* recv_dev, const std::string& topic);
            void createPublisher(const std::string& topic);
            void setTopicCallBack(const std::string& topic, const std::function<void(google::protobuf::Message*)> f);

            template<typename T>
            void loadParam(MutableParam<T>& param, const std::string& key, const T& defaultValue, const std::string& desc = "") {
                T value;
                if (_params.findParam(key, value, defaultValue, true, desc)) {
                    param.init(key, value, &_params, desc);
                }
                else {
                    LogWarn("LoadParam: " << m_device_name << " failed to load mutable param \"" << key << "\", use default value ( " << defaultValue << " )");
                    param.init(key, defaultValue, &_params, desc);
                }
            }

            template<typename T>
            void loadParam(Param<T>& param, const std::string& key, const T& defaultValue, const std::string& desc = "") {
                T value;
                if (_params.findParam(key, value, defaultValue, false, desc)) {
                    param.init(key, value, desc);
                }
                else {
                    LogWarn("LoadParam: " << m_device_name << " failed to load param \"" << key << "\", use default value ( " << defaultValue << " )");
                    param.init(key, defaultValue, desc);
                }
            }

            void loadParam(MutableParam<std::string>& param, const std::string& key, const char* defaultValue, const std::string& desc = "");
            void loadParam(Param<std::string>& param, const std::string& key, const char* defaultValue, const std::string& desc = "");

            template<typename T>
            void loadParam(T& param, const std::string& key, const T& defaultValue, const std::string& desc = "", typename boost::enable_if<boost::mpl::or_<boost::is_integral<T>, boost::is_floating_point<T>>>::type* dummy = 0) {
                if (!_params.findParam(key, param, defaultValue, false)) {
                    param = defaultValue;
                    LogWarn("LoadParam: " << m_device_name << " failed to load param \"" << key << "\", use default value ( " << defaultValue << " )");
                }
            }

            void loadParam(std::string& param, const std::string& key, const char* defaultValue, const std::string& desc = "");

            void saveParam();

            void reloadParam();

            bool isParamChanged();

            template<typename T>
            void publishTopic(const T& data) {
                std::map<std::string, Publisher*>::const_iterator iter = m_publishers.find(T::descriptor()->full_name());
                if (m_publishers.end() == iter) {
                    LogWarn("publishTopic: " << m_device_name << "has not created publisher for topic " << T::descriptor()->full_name());
                }
                else {
                    Publisher* p = iter->second;
                    p->publish(data);
                }
            }

            template<typename T>
            void recordTopic(const T& data) {
                google::protobuf::util::JsonOptions json_options;
                //json_options.always_print_primitive_fields = true;
                std::string json_buffer;
                google::protobuf::util::Status convertStatus = google::protobuf::util::MessageToJsonString(data, &json_buffer, json_options);
                if (convertStatus.ok()) {
                    LogRecord(T::descriptor()->full_name(), json_buffer);
                }
                else {
                    LogError("recordTopic: MessageToJsonString for " << T::descriptor()->full_name() << "is failed, " << convertStatus.ToString());
                }
            }

            template<typename T>
            void getSubscriberData(T& value) {
                std::string topic = T::descriptor()->full_name();
                std::map<std::string, Subscriber*>::const_iterator iter = m_subscribers.find(topic);
                if (m_subscribers.end() == iter) {
                    LogWarn("getSubscriberData: " << m_device_name << "has not subscribed topic " << topic);
                    return;
                }
                iter->second->GetData(value);
            }

            template<typename T>
            void createPublisher() {
                std::string topic = T::descriptor()->full_name();
                std::map<std::string, Publisher*>::const_iterator iter = m_publishers.find(topic);
                if (m_publishers.end() == iter) {
                    Publisher* p = new Publisher(topic, m_device_name);
                    m_publishers.insert(std::make_pair(topic, p));
                }
                else {
                    LogWarn("createPublisher: " << topic << " publisher has been created");
                }
            }

            template<typename T, typename CallBack>
            void setTopicCallBack(CallBack f) {
                std::string topic = T::descriptor()->full_name();
                std::map<std::string, Subscriber*>::const_iterator iter = m_subscribers.find(topic);
                if (m_subscribers.end() == iter) {
                    Subscriber* s = new Subscriber(topic, m_device_name);
                    s->init<T>(f);
                    m_subscribers.insert(std::make_pair(topic, s));
                }
                else {
                    LogWarn("setTopicCallBack: " << topic << " subscriber has been created");
                }
            }

            template<typename T>
            void setTopicCallBack() {
                std::string topic = T::descriptor()->full_name();
                std::map<std::string, Subscriber*>::const_iterator iter = m_subscribers.find(topic);
                if (m_subscribers.end() == iter) {
                    Subscriber* s = new Subscriber(topic, m_device_name);
                    s->init<T>();
                    m_subscribers.insert(std::make_pair(topic, s));
                }
                else {
                    LogWarn("setTopicCallBack: " << topic << " subscriber has been created");
                }
            }

            template<typename T, typename CallBack, typename T2>
            void setTopicCallBack(CallBack f, T2* pthis) {
                std::string topic = T::descriptor()->full_name();
                std::map<std::string, Subscriber*>::const_iterator iter = m_subscribers.find(topic);
                if (m_subscribers.end() == iter) {
                    Subscriber* s = new Subscriber(topic, m_device_name);
                    s->init<T>(f, pthis);
                    m_subscribers.insert(std::make_pair(topic, s));
                }
                else {
                    LogWarn("setTopicCallBack: " << topic << " subscriber has been created");
                }
            }

            template<typename Signature, typename CallBack, typename T2>
            void setServiceCallBack(const std::string& service, CallBack f, T2* pthis) {
                std::map<std::string, BaseService*>::const_iterator iter = m_services.find(service);
                if (m_services.end() == iter) {
                    Service<Signature>* s = new Service<Signature>(service, m_lua_state);
                    s->init(f, pthis);
                    m_services.insert(std::make_pair(service, s));
                }
                else {
                    LogWarn("setServiceCallBack: " << service << " callback has been created");
                }
            }

#if RBK_USE_CPP11
            template<typename ReturnT, typename... Ps>
            ReturnT callService(const std::string& pluginName, const std::string& serviceName, const Ps&... ps) {
                rbk::core::lua::State ss(m_lua_state);
                if (ss["rbk"][pluginName.c_str()].isNilref()) {
                    LogError("callService from C++ error: plugin <" << pluginName << "> is invalid");
                    return ReturnT();
                }
                else {
                    LogInfo("callService from C++: " << serviceName << " @ " << pluginName);
                    try {
                        return ss["rbk"][pluginName.c_str()][serviceName.c_str()].call<ReturnT, const Ps&...>(ps...);
                    }
                    catch (std::exception& e) {
                        LogError("callService from C++ error: " << e.what());
                        return ReturnT();
                    }
                }
            }
#endif

            //template<typename T>
            //void setAttributeCallBack(const std::string& attribute, const T value) {
            //    std::map<std::string, BaseAttribute*>::const_iterator iter = m_attributes.find(attribute);
            //    if (m_attributes.end() == iter) {
            //        Attribute<T>* a = new Attribute<T>(attribute);
            //        a->set(value);
            //        Service<T()>* get_service = new Service<T()>(attribute, m_lua_state);
            //        get_service->init(&Attribute<T>::get, a);
            //        Service<void(T)>* set_service = new Service<void(T)>(std::string("set_").append(attribute), m_lua_state);
            //        set_service->init(&Attribute<T>::set, a);
            //        m_attributes.insert(std::make_pair(attribute, (BaseAttribute*)a));
            //    }
            //    else {
            //        LogWarn("setAttributeCallBack: " << attribute << " callback has been created");
            //    }
            //}

            //template<typename T>
            //void setAttribute(const std::string& attribute, const T value) {
            //    std::map<std::string, BaseAttribute*>::const_iterator iter = m_attributes.find(attribute);
            //    if (m_attributes.end() == iter) {
            //        LogWarn("setAttribute: no this attribute( " << attribute << ")");
            //    }
            //    else {
            //        Attribute<T>* a = reinterpret_cast<Attribute<T>*>(iter->second);
            //        a->set(value);
            //    }
            //}

            //template<typename T>
            //void getAttribute(T& value, const std::string& attribute) {
            //    std::map<std::string, BaseAttribute*>::const_iterator iter = m_attributes.find(attribute);
            //    if (m_attributes.end() == iter) {
            //        LogWarn("getAttribute: no this attribute( " << attribute << ")");
            //    }
            //    else {
            //        Attribute<T>* a = reinterpret_cast<Attribute<T>*>(iter->second);
            //        value = a->get();
            //    }
            //}

            RBK_DEPRECATED void createPluginEvent(const std::string& event_name);

            template <typename T>
            void fireEvent(const T event_name) {
                std::map<std::string, Event*>::iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    Event* e = iter->second;
                    e->fire<T>();
                }
            }

            void bindEvent(const std::string& event_name, const std::string& func_name);

            void unbindEvent(const std::string& event_name, const std::string& func_name);

            void createEvent(const std::string& event_name);

            void fireEvent(const std::string& event_name);

            template<typename P0>
            void fireEvent(const std::string& event_name, const P0& p0) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0>(p0);
                }
            }

            template<typename P0, typename P1>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1>(p0, p1);
                }
            }

            template<typename P0, typename P1, typename P2>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2>(p0, p1, p2);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3>(p0, p1, p2, p3);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4>(p0, p1, p2, p3, p4);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5>(p0, p1, p2, p3, p4, p5);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
            void fireEvent(const std::string& event_name, const  P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6>(p0, p1, p2, p3, p4, p5, p6);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7>(p0, p1, p2, p3, p4, p5, p6, p7);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8>(p0, p1, p2, p3, p4, p5, p6, p7, p8);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12, const P13& p13) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14>
            void fireEvent(const std::string& event_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12, const P13& p13, const P14& p14) {
                std::map<std::string, Event*>::const_iterator iter = m_events.find(event_name);
                if (m_events.end() == iter) {
                    LogWarn("fireEvent: no this event( " << event_name << ")");
                }
                else {
                    iter->second->fire<P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
                }
            }

            void bindDelegate(const std::string& delegate_name, const std::string& func_name);

            void unbindDelegate(const std::string& delegate_name, const std::string& func_name);

            template<typename T>
            void callDelegate(const T delegate_name) {
                std::map<std::string, Delegate*>::iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    Delegate* d = iter->second;
                    d->call<boost::tuple<>>(); // trick
                }
            }

            bool delegateLuaFuncExists(const std::string& delegate_name);

            void createDelegate(const std::string& delegate_name);

            // R 里面不能用指针类型，如 char*, char* 返回后仍然是 nullptr
            template<typename R>
            R callDelegate(const std::string& delegate_name) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R>();
                }
            }

            template<typename P0>
            void callDelegate(const std::string& delegate_name, const P0& p0) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0);
                }
            }

            template<typename R, typename P0>
            R callDelegate(const std::string& delegate_name, const P0& p0) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0>(p0);
                }
            }

            template<typename P0, typename P1>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1);
                }
            }

            template<typename R, typename P0, typename P1>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1>(p0, p1);
                }
            }

            template<typename P0, typename P1, typename P2>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2);
                }
            }

            template<typename R, typename P0, typename P1, typename P2>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2>(p0, p1, p2);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3>(p0, p1, p2, p3);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4>(p0, p1, p2, p3, p4);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5>(p0, p1, p2, p3, p4, p5);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6>(p0, p1, p2, p3, p4, p5, p6);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7>(p0, p1, p2, p3, p4, p5, p6, p7);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8>(p0, p1, p2, p3, p4, p5, p6, p7, p8);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12, const P13& p13) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12, const P13& p13) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
                }
            }

            template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14>
            void callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12, const P13& p13, const P14& p14) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                }
                else {
                    iter->second->call(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
                }
            }

            template<typename R, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14>
            R callDelegate(const std::string& delegate_name, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P9& p9, const P10& p10, const P11& p11, const P12& p12, const P13& p13, const P14& p14) {
                std::map<std::string, Delegate*>::const_iterator iter = m_delegates.find(delegate_name);
                if (m_delegates.end() == iter) {
                    LogWarn("callDelegate: no this delegate( " << delegate_name << ")");
                    return R();
                }
                else {
                    return iter->second->call<R, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14>(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
                }
            }
        };

        //template<>
        //RBK_API void NPlugin::setAttributeCallBack<int>(const std::string& attribute, const int value);

        //template<>
        //RBK_API void NPlugin::setAttributeCallBack<const char*>(const std::string& attribute, const char* value);

        //template<>
        //RBK_API void NPlugin::setAttribute<int>(const std::string& attribute, const int value);

        //template<>
        //RBK_API void NPlugin::setAttribute<const char*>(const std::string& attribute, const char* value);

        //template<>
        //RBK_API void NPlugin::getAttribute(int& value, const std::string& attribute);
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_PLUGIN_H_
