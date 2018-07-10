#include "RadioPackets.h"
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

namespace{
	int the_sign(double d){ return (d >= 0) ? 1 : -1; }
	HANDLE m_hEvent;
}

RadioPacketV2011::RadioPacketV2011() {
	send_packet_len = 24;
	recv_packet_len = 18;
	
	TXBuff = new BYTE[send_packet_len+1];
	current_pgknum = 0;
	m_hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
}

RadioPacketV2011::~RadioPacketV2011()
{
	delete [] TXBuff;
}

void RadioPacketV2011::encode(rbk::protocol::SRC_Cmd& robotCommand, rbk::utils::serialport::AsyncSerial* pSerial)
{
	///> 1.获取按序排列的小车
	// 获取图像收到的车号
	std::vector<int> num_list;
	num_list.clear();
	for (int i = 0; i < robotCommand.command_size(); i++) {
		rbk::protocol::Message_SSL_Command cmd = robotCommand.command(i);
		int vision_num = cmd.robot_id() - 1;
		if (vision_num >= 0 && vision_num <= 11) {
			num_list.push_back(vision_num);
			rcmd[vision_num] = cmd;
		}
	}
	// 按车号进行排序
	sort(num_list.begin(), num_list.end());

	///> 2.根据协议模式进行编码及下发
	if (num_list.size() <= 3) {
		current_pgknum = 0;
		this->encodeNormal(num_list, pSerial);
		WaitForSingleObject(m_hEvent,8);
		current_pgknum = 1;
		this->encodeNormal(num_list, pSerial);
	} else{
		current_pgknum = 0;
		this->encodeNormal(num_list, pSerial);
		WaitForSingleObject(m_hEvent,8);
		current_pgknum = 1;
		this->encodeNormal(num_list, pSerial);
	}

	return ;
}

void RadioPacketV2011::encodeNormal(const std::vector<int>& robots, rbk::utils::serialport::AsyncSerial* pSerial)
{
	// ！！robots 是按照车号排序的！！
	// 先根据当时场上车的数量决定下发包的个数
	int packet_num = 3;

	memset(TXBuff, 0, send_packet_len+1);
	int robots_size = robots.size();
	if (0 == current_pgknum) {
		if (3 < robots.size()) {
			robots_size = 3;
		}
	} else {
		if (3 < robots.size()) {
			robots_size = robots.size() - 3;
		} else {
			robots_size = 0;
		}
	}

	// 编码下发
	TXBuff[0] = 0xFF;
	for (unsigned int i = 0; i < robots_size; i++){
		int real_num = robots[i+current_pgknum*packet_num];
		
		// num
		if (real_num > 7) {
			TXBuff[1] += (1 << (real_num-8)) & 0xF;
		} else {
			TXBuff[2] += (1 << (real_num)) & 0xFF;
		}

		// ctrl & shoot
		if (rcmd[real_num].flat_kick() > 0) {
			TXBuff[i*4+3] = (0 << 6) & 0xFF;
		} else if (rcmd[real_num].chip_kick() > 0) {
			TXBuff[i*4+3] = (1 << 6) & 0xFF;
		}
		//std::cout<<"cccccc "<<real_num<<" "<<rcmd[real_num].dribbler_spin()<<" "<<rcmd[real_num].chip_kick()<<std::endl;
		if (rcmd[real_num].dribbler_spin() < 0) {
			TXBuff[i*4+3] += 0x80;
		}

		int dribble_power = abs(rcmd[real_num].dribbler_spin() * 3);
		TXBuff[i*4+3] += ((dribble_power & 0x3 ) << 4);
		
		// vx
		double v_x = abs(rcmd[real_num].velocity_x()*100)>511 ? the_sign(rcmd[real_num].velocity_x())*511 : rcmd[real_num].velocity_x()*100;
		unsigned int vx_value_uint = abs(v_x);
		unsigned char vx_value_uchar = abs(v_x);
	//	std::cout<<"v_x : "<<v_x<<endl;
		TXBuff[i*4+4] = (vx_value_uint & 0x7F);
		if (TXBuff[i*4+4] == 0x7F) {
			TXBuff[i*4+4] = 0x7E;
		}
		TXBuff[i*4+4] |= (v_x>=0)?0:0x80;
		TXBuff[i*1+15] |= ((vx_value_uint >> 7) & 0x3) << 6;

		// vy 
		double v_y = abs(rcmd[real_num].velocity_y()*100)>511 ? the_sign(rcmd[real_num].velocity_y())*511 : rcmd[real_num].velocity_y()*100;
		v_y = 1.0 * v_y;
	//	std::cout<<"v_y : "<<v_y<<endl;
		unsigned int vy_value_uint = abs(v_y);
		unsigned char vy_value_uchar = abs(v_y);
		TXBuff[i*4+5] = (vy_value_uint & 0x7F);
		if (TXBuff[i*4+5] == 0x7F) {
			TXBuff[i*4+5] = 0x7E;
		}
		TXBuff[i*4+5] |= (v_y>=0)?0:0x80;
		TXBuff[i*1+15] |= ((vy_value_uint >> 7) & 0x3) << 4;

		// w
		double v_w = (abs(rcmd[real_num].velocity_r()*40) > 500 ) ? the_sign(rcmd[real_num].velocity_r())*500 : rcmd[real_num].velocity_r()*40;
		v_w = 1.0 * v_w;
	//	std::cout<<"v_w : "<<v_w<<endl;
		unsigned int w_value_uint = abs(v_w);
		unsigned char w_value_uchar = abs(v_w);
		TXBuff[i*4+6] = (w_value_uint & 0x7F);
		if (TXBuff[i*4+6] == 0x7F) {
			TXBuff[i*4+6] = 0x7E;
		}
		TXBuff[i*4+6] |= (v_w>=0)?0:0x80;
		TXBuff[i*1+15] |= ((w_value_uint >> 7) & 0x3) << 2;

		// kick pow
		if (rcmd[real_num].flat_kick() > 0) {
			double flat_power = rcmd[real_num].flat_kick();
			if (flat_power >= 127) {
				flat_power = 127;
			}
			unsigned char kick_value_uchar = abs(flat_power);
			TXBuff[i*1+18] = kick_value_uchar & 0x7F;
		} else if (rcmd[real_num].chip_kick() > 0) {
			double chip_power = rcmd[real_num].chip_kick();
			if (chip_power >= 127) {
				chip_power = 127;
			}
			unsigned char kick_value_uchar = abs(chip_power);
			TXBuff[i*1+18] = kick_value_uchar & 0x7F;
		} else{
			TXBuff[i*1+18] = 0x00 & 0x7F;
		}
	}
	// Send
	pSerial->write((char*)TXBuff, send_packet_len+1);
	//for(int i = 0; i < 25; i++){
	//	printf("%0x ",TXBuff[i]);
	//}
	//cout<<endl;
}