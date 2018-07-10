#ifndef __SSL_GUIDEBUGER_H__
#define __SSL_GUIDEBUGER_H__

#include <robokit/core/rbk_core.h>
#include <robokit/utils/udp/udp_client.h>
#include "src_debug.pb.h"
using namespace rbk;

class SSLGuiDebugger : public NPluginInterface {
public:
	SSLGuiDebugger();
	~SSLGuiDebugger();
	void run();
	void loadFromConfigFile();
	void setSubscriberCallBack();
	void messageTestCallBack(google::protobuf::Message*);
private:
	void send(const rbk::protocol::Debug_Msgs&);
	rbk::utils::udp::SyncUDPClient gui_socket;
	int gui_port;
	int gui_bind_port;
	bool send_debug_msg;
};

RBK_INHERIT_PROVIDER(SSLGuiDebugger,NPluginInterface,"1.0.0");

#endif