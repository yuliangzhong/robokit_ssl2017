#ifndef __WIRELESS_RADIO_PACKETS_H__
#define __WIRELESS_RADIO_PACKETS_H__

#include "src_cmd.pb.h"
#include <robokit/utils/serial_port.h>
#include <wtypes.h>
#include <vector>
#include <fstream>

class NSerialPort;
class RadioPacketV2011 {
public:
	RadioPacketV2011();
	~RadioPacketV2011();
	void encode(rbk::protocol::SRC_Cmd& robotCommand, rbk::utils::serialport::AsyncSerial* pSerial);
	void setpkgnum(int pkgNum) { current_pgknum = pkgNum;}
private:
	void encodeNormal(const std::vector<int>& robots, rbk::utils::serialport::AsyncSerial* pSerial);
	rbk::protocol::Message_SSL_Command rcmd[12];
	int current_pgknum;
	int send_packet_len;
	int recv_packet_len;
	BYTE* TXBuff;
};

#endif