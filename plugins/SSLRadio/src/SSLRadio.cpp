#include "SSLRadio.h"
#include <iostream>
#include <vector>
#include "RadioPackets.h"
#include  <MMSystem.h>
#include "robokit/core/model.h"
#include <boost/lexical_cast.hpp>
#include <cmath>
RBK_INHERIT_SOURCE(SSLRadio)

namespace{
	//NMutex m_mutex;
}

SSLRadio::SSLRadio(){
	for(int i=0;i<12;i++){
		rbk::protocol::Robot_Status* status = robots_status.add_robots_status();
		status->set_robot(i+1);
		//status[i]->set_infrared(false);
		//status[i]->set_flat_kick(false);
		//status[i]->set_chip_kick(false);
		//status[i]->set_controlled(false);
		//status[i]->set_change_num(0);
		//status[i]->set_change_cnt(0);
	}
	m_recv_back = false;
	m_init = false;
}

SSLRadio::~SSLRadio()
{
	delete m_serial_port;
}

// 用于接受外部的指令
/*void __cdecl beginThreadFunc(void * lp)
{
	NUDPSocket m_socket;
	m_socket.init(10010,true);
	char recv_msg[2048];
	SSLRadio* ssl_robot = (SSLRadio*)lp;
	while(true){
	memset(recv_msg,0,2048);
	int recv_size = 0;
	if (m_socket.receiveData(recv_msg,2048,recv_size)) {
	m_mutex.Lock();
	ssl_robot->m_ssl.ParseFromArray(recv_msg, recv_size);
	m_mutex.Unlock();
	}
	}
}*/

namespace{
	int last_size = 0;
}

void SSLRadio::readFunc(const char* buf_recv, size_t size)
{
	if (m_init) {
		unsigned char* buf = (unsigned char*)buf_recv;
		//for(int i = 0; i < size; i++){
		//	printf("%02x ", buf[i]);
		//}
		//printf("\n");
		for (unsigned int i = 0; i < size; i++) {
			if (buf[i] == 0xff && i + 5 < size) {
				if (buf[i+1] == 0x02) {
					unsigned int robot_id = (buf[i+2] & 0x0f) - 1;
					if(robot_id > 11){
						break;
					}
					//std::cout<<robot_id<<std::endl;
					robots_status.mutable_robots_status(robot_id)->set_infrared((buf[i+3] & 0x40) > 0);
					robots_status.mutable_robots_status(robot_id)->set_flat_kick((buf[i+3] & 0x20) >> 5 > 0 );
					robots_status.mutable_robots_status(robot_id)->set_chip_kick((buf[i+3] & 0x10) >> 4 > 0);
					robots_status.mutable_robots_status(robot_id)->set_controlled((buf[i+3] & 0x08) >> 3);
					robots_status.mutable_robots_status(robot_id)->set_change_num(buf[i+4] & 0xff);
					robots_status.mutable_robots_status(robot_id)->set_change_cnt(buf[i+5] & 0x0f);
					//	std::cout <<"CCC "<< robots_status.robots_status(0).change_num()<<" "<< robots_status.robots_status(0).infrared() << '\t' << robots_status.robots_status(0).flat_kick() << '\t' << robots_status.robots_status(0).chip_kick() << std::endl;
					m_recv_back = true;
					break;
				}
			} else{
				//LogError("Not continued recv packet");
			}
		}
		publishTopic(robots_status);
	}
}

void SSLRadio::run() {
	static int count = 0;
	//_beginthread(beginThreadFunc, 0, this);
	timeBeginPeriod(1);
	HANDLE m_hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_serial_port = new rbk::utils::serialport::AsyncSerial();
	m_serial_port->setReadCallback(boost::bind(&SSLRadio::readFunc,this,boost::placeholders::_1, boost::placeholders::_2));
	try{
		m_serial_port->open(serialPortName, 115200);
	} catch( boost::system::system_error& e){
		LogError("Open Com Error: "<<e.what());
		return ;
	}
	

	m_init = m_serial_port->isOpen();
	if (m_init) {
		LogInfo("Connect Radio Successfully!!");
	}
	m_radio_packet = new RadioPacketV2011();

	while(true){
	//	count ++;
	//	if(m_recv_back){
	//		publishTopic(robots_status);
	//		m_recv_back = false;
	//	}
	//	//////////////////
	//	if (count%10==0) {
	//		//std::cout << robots_status.robots_status(0).infrared() << '\t' << robots_status.robots_status(0).flat_kick() << '\t' << robots_status.robots_status(0).chip_kick() << std::endl;
	//	}
		rbk::protocol::SRC_Cmd cmds;
		getSubscriberData(cmds);
		if(cmds.command_size()>0){
			//std::cout<<ssl.DebugString()<<std::endl;
			convertKickPower(cmds);
			m_radio_packet->encode(cmds,m_serial_port);
		}

		WaitForSingleObject(m_hEvent, 8);
	//	//////////////////
	}
}

void SSLRadio::loadFromConfigFile(){
	loadParam(serialPortName,"SerialPortName","COM3");

	// read kickParams.cfg
	auto model_doc = rbk::Model::Instance()->getJson();
	std::string flat_a("/FLAT_A/");
	std::string flat_b("/FLAT_B/");
	std::string flat_c("/FLAT_C/");
	std::string flat_max("/FLAT_MAX/");
	std::string flat_min("/FLAT_MIN/");
	std::string chip_a("/CHIP_A/");
	std::string chip_b("/CHIP_B/");
	std::string chip_c("/CHIP_C/");
	std::string chip_max("/CHIP_MAX/");
	std::string chip_min("/CHIP_MIN/");
	for(int i=0;i<12;i++){
		rbk::json::findByPointer(*model_doc,flat_a  +boost::lexical_cast<std::string>(i),params[i].FLAT.A);
		rbk::json::findByPointer(*model_doc,flat_b  +boost::lexical_cast<std::string>(i),params[i].FLAT.B);
		rbk::json::findByPointer(*model_doc,flat_c  +boost::lexical_cast<std::string>(i),params[i].FLAT.C);
		rbk::json::findByPointer(*model_doc,flat_max+boost::lexical_cast<std::string>(i),params[i].FLAT.MAX);
		rbk::json::findByPointer(*model_doc,flat_min+boost::lexical_cast<std::string>(i),params[i].FLAT.MIN);
		rbk::json::findByPointer(*model_doc,chip_a  +boost::lexical_cast<std::string>(i),params[i].CHIP.A);
		rbk::json::findByPointer(*model_doc,chip_b  +boost::lexical_cast<std::string>(i),params[i].CHIP.B);
		rbk::json::findByPointer(*model_doc,chip_c  +boost::lexical_cast<std::string>(i),params[i].CHIP.C);
		rbk::json::findByPointer(*model_doc,chip_max+boost::lexical_cast<std::string>(i),params[i].CHIP.MAX);
		rbk::json::findByPointer(*model_doc,chip_min+boost::lexical_cast<std::string>(i),params[i].CHIP.MIN);
	}
}

void SSLRadio::setSubscriberCallBack()
{
	setTopicCallBack<rbk::protocol::SRC_Cmd>(&SSLRadio::messageSSLCallBack,this);
	createPublisher<rbk::protocol::Robots_Status>();
}

void SSLRadio::messageSSLCallBack(google::protobuf::Message* ssl){

}
void SSLRadio::convertKickPower(rbk::protocol::SRC_Cmd& cmds){
	for(int i=0;i<cmds.command_size();i++){
		auto& cmd = cmds.command(i);
		auto flatCmd = cmd.flat_kick()*100;// m/s -> cm/s ;
		auto chipCmd = cmd.chip_kick()*100;// m -> cm ;
		auto robotNum = cmd.robot_id() - 1;// 1~12 -> 0~11 ;
		unsigned int chipPower = 0;
		unsigned int flatPower = 0;
		if(fabs(flatCmd) < 0.01) {
			if(fabs(chipCmd) > 0) {
				chipPower = chip(robotNum,chipCmd);
			}
		}else{
			flatPower = flat(robotNum,flatCmd);
		}
		/*if (robotNum == 3) {
			std::cout<<flatPower<<" "<<chipPower<<std::endl;
		}*/
		cmds.mutable_command(i)->set_flat_kick(flatPower);
		cmds.mutable_command(i)->set_chip_kick(chipPower);
	}
}
unsigned int SSLRadio::chip(unsigned int num,double kick){
	if(num > 11) return 0;
	int shootpower = 0;
	auto P = params[num].CHIP;
	shootpower = (int)(P.A*kick*kick + P.B*kick + P.C);
	
	if (fabs(kick - 5555) < 127) {
		return fabs(kick - 5555);
	}

	if (shootpower > P.MAX || kick>400) {
		shootpower = P.MAX;
	} else if (shootpower < P.MIN) {
		shootpower = P.MIN;
	}
	return shootpower;
}
unsigned int SSLRadio::flat(unsigned int num,double kick){
	if(num > 11) return 0;
	// TODO 二次函数拟合，A是二次项系数，B是一次项系数，C是常数项
	// 二次项系数乘了0.00001，这里要特别注意，yys 2015-05-02
	auto P = params[num].FLAT;
	int shootpower = 0;
	shootpower = (int)(0.00001*P.A*kick*kick + P.B*kick + P.C);
	if (kick >= 9999) {
		return 127;
	}

	if (fabs(kick - 8888) < 0.5) {
		return 70;
	}
	if (fabs(kick - 5555) < 127) {
		return fabs(kick - 5555);
	}
	if (shootpower > P.MAX) {
		shootpower = P.MAX;
	} else if (shootpower < P.MIN) {
		shootpower = P.MIN;
	}
	return shootpower;
}