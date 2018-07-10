#ifndef _SSL_REFEREE_BOX_H_
#define _SSL_REFEREE_BOX_H_

#include <robokit/core/rbk_core.h>
#include <robokit/utils/udp/udp_group.h>
#include "referee.pb.h"

using namespace rbk;

class SSLRefereeBox : public NPluginInterface
{
public:
	SSLRefereeBox();
	~SSLRefereeBox();
	void run();
	void loadFromConfigFile();
	void setSubscriberCallBack();
private:
	rbk::utils::udp::UDPGroupReceiver _group_receiver;
	rbk::MutableParam<std::string> m_referee_ip;
	rbk::MutableParam<int> m_referee_port;
};

RBK_INHERIT_PROVIDER(SSLRefereeBox, NPluginInterface, "1.0.0");

#endif // ~_SSL_REFEREE_BOX_H_