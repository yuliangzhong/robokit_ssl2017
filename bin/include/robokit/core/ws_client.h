#ifndef _RBK_CORE_WS_CLIENT_H_
#define _RBK_CORE_WS_CLIENT_H_

#include <robokit/config.h>
#include <map>
#include <string>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

namespace rbk {
    namespace core {
        typedef websocketpp::client<websocketpp::config::asio_client> WebSocketClient;
        typedef websocketpp::connection_hdl WebSocketHandle;
        class RBK_DLL_EXPORT WSClient {
        public:
            WSClient();
            WSClient(std::string, int, std::string);
            ~WSClient();
            void init(std::string, int, std::string);
            void setMessageHandler(boost::function<void(WebSocketHandle, WebSocketClient::message_ptr)>);
            void setCloseHandler(boost::function<void(WebSocketHandle)>);
            void setFailHandler(boost::function<void(WebSocketHandle)>);
            void setOpenHandler(boost::function<void(WebSocketHandle)>);
            void setRunFunction(boost::function<void()>);
            void addHeader(std::string, std::string);
            void addCookie(std::string, std::string);
            void startConnect();
            void sendData(std::string);
            bool connected();
            std::string getRemoteIP();
            int getRemotePort();
            std::string getLocalIP();
            void setMsgProto(std::string);
            std::string getMsgProto();
            void setUUID(std::string);
            std::string getUUID();
        private:
            void runWSClientThread();
            std::string m_server_ip;
            int	m_server_port;
            std::string m_uuid;
            std::string m_msg_proto;
            std::string m_server_route;
            WebSocketClient m_client;
            volatile bool m_server_closed;
            volatile bool m_send_connection;
            volatile bool m_init;
            volatile bool m_thread_end;
            std::map<std::string, std::string> m_header_map;
            std::map<std::string, std::string> m_cookie_map;
            WebSocketClient::connection_ptr m_con;
            boost::mutex m_con_mutex;
            boost::function<void(WebSocketHandle, WebSocketClient::message_ptr)> m_message_handler;
            boost::function<void(WebSocketHandle)> m_close_handler;
            boost::function<void(WebSocketHandle)> m_fail_handler;
            boost::function<void(WebSocketHandle)> m_open_handler;
            boost::function<void()> m_run_function;
        };
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_WS_CLIENT_H_
