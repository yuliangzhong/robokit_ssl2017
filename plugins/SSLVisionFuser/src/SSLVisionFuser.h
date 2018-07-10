#ifndef __SSL_VISION_MEDIATOR_H__
#define __SSL_VISION_MEDIATOR_H__

#include <robokit/core/rbk_core.h>
#include <robokit/utils/udp/udp_group.h>
#include "params.h"
#include "./follow/Transmit.h"
#include "./follow/BallFollow.h"
#include "./follow/RobotFollow.h"
#include "messages_robocup_ssl_wrapper.pb.h"
using namespace rbk;

class SSLVisionFuser : public NPluginInterface {
public:
	SSLVisionFuser();
	~SSLVisionFuser();
	void run();
	void loadFromConfigFile();
	void setSubscriberCallBack();
	//void messageTestCallBack(google::protobuf::Message*);
public:
	void parse(void*,int);
private:
	rbk::utils::udp::UDPGroupReceiver udpServer;
	rbk::utils::udp::UDPGroupSender udpClient;

	rbk::MutableParam<std::string> m_vision_ip;
	rbk::MutableParam<int> m_vision_port;
	rbk::MutableParam<double> m_distorterr;
	rbk::MutableParam<double> m_maxBallSpeed;
	rbk::MutableParam<double> m_maxVehicleSpeed;
	rbk::MutableParam<int> m_minAddFrame;
	rbk::MutableParam<int> m_minLostFrame;
	rbk::MutableParam<int> m_followCheckCycle;
	rbk::MutableParam<int> m_fuser_port;
	double m_maxBallDist;
	double m_maxVehicleDist;
	rbk::MutableParam<bool> cameraControl_1;
	rbk::MutableParam<bool> cameraControl_2;
	rbk::MutableParam<bool> cameraControl_3;
	rbk::MutableParam<bool> cameraControl_4;
	bool cameraControl[4];
private:
	bool collectNewVision();
	void sendSmsg();
	bool cameraUpdate[PARAM::CAMERA];
	BallFollow ballFollow;
	RobotFollow yellowFollow;
	RobotFollow blueFollow;

	void setNewCameraID(int id);

	SendVisionMessage  transmit_msg;
	SSL_DetectionFrame detections[PARAM::CAMERA];
	int newCameraID;
	int _cycle;
	CAMERAMODE cameraMode;
	bool m_sendFalse;//false
	Transmit transmit;

};

RBK_INHERIT_PROVIDER(SSLVisionFuser,NPluginInterface,"1.0.0");

#endif