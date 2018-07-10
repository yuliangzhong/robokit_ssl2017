#ifndef _RBK_CORE_WS_SERVER_H_
#define _RBK_CORE_WS_SERVER_H_

#include <robokit/config.h>
#include <map>
#include <string>
#include <set>
#include <boost/random.hpp>
#include <robokit/core/ws_client.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace rbk {
    namespace core {
        typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;
        typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> WebSocketConList;

        class RBK_DLL_EXPORT RandomPort {
        public:
            RandomPort();
            int getRandomPort(int min, int max);
        private:
            bool isPortFree(int port);
            boost::asio::io_service m_io_service;
        };

        class RBK_DLL_EXPORT WSServer {
        public:
            WSServer();
            WSServer(int);
            ~WSServer();
            void init(int);
            void setMessageHandler(boost::function<void(WebSocketHandle, WebSocketServer::message_ptr)>);
            void setCloseHandler(boost::function<void(WebSocketHandle)>);
            void setFailHandler(boost::function<void(WebSocketHandle)>);
            void setOpenHandler(boost::function<void(WebSocketHandle)>);
            void startServer();
            void sendData(std::string);
            std::string getHeader(WebSocketHandle, std::string);
            std::string getCookie(WebSocketHandle, std::string);
            std::string getLocalIP();
            int getLocalPort();
            void setMsgProto(std::string);
            std::string getMsgProto();
        private:
            int m_port;
            std::string m_ip;
            std::string m_proto;
            boost::mutex m_con_mutex;
            WebSocketServer m_server;
            WebSocketConList m_con_list;
            boost::function<void(WebSocketHandle, WebSocketServer::message_ptr)> m_message_handler;
            boost::function<void(WebSocketHandle)> m_close_handler;
            boost::function<void(WebSocketHandle)> m_fail_handler;
            boost::function<void(WebSocketHandle)> m_open_handler;
        };
    } // namespace core
} // namespace rbk

#endif // ~_RBK_CORE_WS_SERVER_H_
