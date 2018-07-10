#include "SSLVisionFuser.h"
#include "src_rawvision.pb.h"
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"
RBK_INHERIT_SOURCE(SSLVisionFuser)

SSLVisionFuser::SSLVisionFuser(void)
	:cameraMode(FourCamera),m_sendFalse(false){
}
   
SSLVisionFuser::~SSLVisionFuser(void){
	udpServer.close();
}

void SSLVisionFuser::run(){
	static int count = 0;
	udpServer.listen(m_vision_ip.c_str(),m_vision_port, true);
	udpClient.bind("127.0.0.1",m_fuser_port);
	while(true){
		char buffer[65536];
		int len = sizeof(buffer);
		int receive_byteCnt = udpServer.read(buffer, len);
		if(receive_byteCnt > 0){
			parse((void *)buffer,receive_byteCnt);
		}
	}
}
void SSLVisionFuser::loadFromConfigFile(){
	loadParam(m_vision_ip,"VisionIP", "224.5.23.2");
	loadParam(m_vision_port,"VisionPort", 10005);
	loadParam(m_fuser_port,"VisionTransPort",54321);
	loadParam(m_distorterr,"VisionDistortERR",25.0);
	loadParam(m_maxBallSpeed,"VisionMaxBallSpeed",10.0);
	loadParam(m_maxVehicleSpeed,"VisionMaxVehicleSpeed",5.0);
	loadParam(m_minAddFrame,"VisionMinAddFrame",3);
	loadParam(m_minLostFrame,"VisionMinLostFrame",10);
	loadParam(m_followCheckCycle,"VisionFollowCheckCycle",10);
	loadParam(cameraControl_1,"VisionCameraControl_1",true);
	loadParam(cameraControl_2,"VisionCameraControl_2",true);
	loadParam(cameraControl_3,"VisionCameraControl_3",true);
	loadParam(cameraControl_4,"VisionCameraControl_4",true);
	cameraControl[0] = cameraControl_1;
	cameraControl[1] = cameraControl_2;
	cameraControl[2] = cameraControl_3;
	cameraControl[3] = cameraControl_4;
	for(int i=0;i<4;i++){
		if(cameraControl[i] == false){
			LogWarn("Not Full Vision Mode!!!");
			break;
		}
	}
	m_maxBallDist = m_maxBallSpeed*100/60;
	m_maxVehicleDist = m_maxVehicleSpeed*100/60;
}
void SSLVisionFuser::setSubscriberCallBack(){
	createPublisher<rbk::protocol::SRC_RawVision>();
}
void SSLVisionFuser::setNewCameraID(int id){
	newCameraID = id;
	cameraUpdate[id] = true;
}
bool SSLVisionFuser::collectNewVision() {
	for (int i=0;i<PARAM::CAMERA;i++) {
		if (cameraControl[i] && !cameraUpdate[i])
			return false;
	}
	return true;
}
//void SSLVisionMediator::messageTestCallBack(google::protobuf::Message* msg)
void SSLVisionFuser::parse(void * ptr,int size){
	SSL_WrapperPacket packet;
	packet.ParseFromArray(ptr,size);
	if (packet.has_detection()) {
		int id = packet.detection().camera_id();
		setNewCameraID(id);
		if(cameraControl[id]){
			detections[id] = packet.detection();
			int balls_n = detections[id].balls_size();
			int robots_blue_n =  detections[id].robots_blue_size();
			int robots_yellow_n =  detections[id].robots_yellow_size();
			for (int i = 0; i < balls_n; i++) {
				SSL_DetectionBall ball = detections[id].balls(i);
				GlobalData::instance()->setBall(newCameraID, i, ball.x(),ball.y(),ball.confidence());
			}
			for (int i = 0; i < robots_blue_n; i++) {
				SSL_DetectionRobot robot = detections[id].robots_blue(i);
				GlobalData::instance()->setBlueRobot(newCameraID, i,robot.robot_id(),robot.x(),robot.y(),robot.orientation(),robot.confidence());
			}
			for (int i = 0; i < robots_yellow_n; i++) {
				SSL_DetectionRobot robot = detections[id].robots_yellow(i);
				GlobalData::instance()->setYellowRobot(newCameraID, i,robot.robot_id(),robot.x(),robot.y(),robot.orientation(),robot.confidence());
			}
			if (collectNewVision()) {
				if(GlobalData::instance()->smsg.Cycle % 1 == 0)
					GlobalData::instance()->copyReceiveMsg();
				if(_cycle > 32766)
					_cycle = 1;
				transmit.setCycle(m_followCheckCycle, _cycle++);
				sendSmsg();
				std::fill_n(cameraUpdate,PARAM::CAMERA,false);
			}
		}
	}
}
void SSLVisionFuser::sendSmsg(){
	transmit_msg = transmit.smsgUpdate(cameraMode,m_minAddFrame, m_minLostFrame,m_maxVehicleDist + m_distorterr,m_sendFalse);
	auto& input = transmit_msg;
	rbk::protocol::SRC_RawVision msg;
	msg.set_frame_number(transmit_msg.Cycle);
	
	rbk::protocol::Ball* ball = msg.mutable_ball();
	ball->set_x(input.Ballx);
	ball->set_y(input.Bally);
	ball->set_ball_found(input.BallFound);
	ball->set_image_x(input.BallImagex);
	ball->set_image_y(input.BallImagey);
	ball->set_image_valid(transmit_msg.nCameraID);
	for(int i=0;i<PARAM::ROBOTNUM;i++){
		{
			rbk::protocol::Robot* robot = msg.add_robots_blue();
			robot->set_robot_found(input.RobotFound[BLUE][i]);
			robot->set_robot_id(input.RobotINDEX[BLUE][i]);
			robot->set_x(input.RobotPosX[BLUE][i]);
			robot->set_y(input.RobotPosY[BLUE][i]);
			robot->set_orientation(input.RobotRotation[BLUE][i]);
		}
		{
			rbk::protocol::Robot* robot =msg.add_robots_yellow();
			robot->set_robot_found(input.RobotFound[YELLOW][i]);
			robot->set_robot_id(input.RobotINDEX[YELLOW][i]);
			robot->set_x(input.RobotPosX[YELLOW][i]);
			robot->set_y(input.RobotPosY[YELLOW][i]);
			robot->set_orientation(input.RobotRotation[YELLOW][i]);
		}
	}
	//for(int i=0;i<6;i++){
	//	std::cout << input.RobotRotation[YELLOW][i] << ' ' << input.RobotRotation[BLUE][i] << std::endl;
	//}
	publishTopic(msg);
	udpClient.write((char*)&transmit_msg,sizeof(transmit_msg));
	//GlobalData::instance()->msg.push(transmit_msg);
	//sendUdp->writeDatagram((char*)&transmit_msg,sizeof(transmit_msg),sendAddress, sendPort);
	//sendUdp->writeDatagram((char*)&transmit_msg,sizeof(transmit_msg),sendAddress2, sendPort2);
}