#ifndef _SSL_RADIO_H_
#define _SSL_RADIO_H_

#include <robokit/utils/serial_port.h>
#include <robokit/core/rbk_core.h>
#include "src_cmd.pb.h"

using namespace rbk;

class RadioPacketV2011;
class SSLRadio : public NPluginInterface
{
public:
	SSLRadio();
	~SSLRadio();
	void run();
	void loadFromConfigFile();
	void setSubscriberCallBack();
	rbk::utils::serialport::AsyncSerial* m_serial_port;
	RadioPacketV2011* m_radio_packet;
	void messageSSLCallBack(google::protobuf::Message*);
	void readFunc(const char* buf_recv, size_t size);
private:
	struct KickParam{
		double A;
		double B;
		double C;
		int MAX;
		int MIN;
	};
	struct KickParams{
		KickParam FLAT;
		KickParam CHIP;
	};
	KickParams params[12];
	void convertKickPower(rbk::protocol::SRC_Cmd&);
	unsigned int flat(unsigned int,double);
	unsigned int chip(unsigned int,double);
	rbk::protocol::Robots_Status robots_status;
	std::string m_ip;
	rbk::MutableParam<std::string> serialPortName;
	volatile bool m_recv_back;
	volatile bool m_init;
};

RBK_INHERIT_PROVIDER(SSLRadio, NPluginInterface, "1.0.0");

#endif // ~_SSL_RADIO_H_