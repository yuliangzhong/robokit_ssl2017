#define _WIN32_DCOM
#include "SSLSimulator.h"
#include "src_rawvision.pb.h"
#include "src_cmd.pb.h"
#include <iostream>
#include <math.h>
#include <robokit/core/rbk_config.h>

#if defined(RBK_SYS_WINDOWS) && _MSC_VER <= 1600
#include <boost/chrono.hpp>
#else
#include <chrono>
#endif

RBK_INHERIT_SOURCE(SSLSimulator)

enum CommandTypeT{ CTStop, CTDash, CTTurn, CTArc, CTSpeed, CTKick };

namespace{
	const int TEAM_BLUE = (1 << 8);
	const int TEAM_YELLOW = (1 << 9);

	IPlayer* pPlayer = 0;
	ITeam* pTeam = 0;
	IViewer* pViewer = 0;
	IController* pController = 0;
	// TODO MAX_PLAYER
	bool stopCommandSent[6]; // 是否发送了停止命令
	int mySide = 1; //1左 -1右
	int myColor = TEAM_BLUE;
	int myType = 2;
	bool isYellowTeam = false;
	float infoX2msgX(float x) { return (x*10.0); }
	float infoY2msgY(float y) { return (-y*10.0); }
	float infoAngle2msgAngle(float angle) { return - angle * 180.0 / (atan(1.0)*4); }
	float velTrans(double vel) { return vel * 180.0 / (atan(1.0)*4); }
	void sendRawVision(const SSLSimulator::VisualInfo& info);
	void recCommandAndSend();
}

SSLSimulator::SSLSimulator()
{
	rbk::Config::Instance()->get("yellowTeam", isYellowTeam);
	if (isYellowTeam) {
		myColor = TEAM_YELLOW;
		mySide = -1;
	}

	if( CoInitializeEx(NULL, COINIT_MULTITHREADED) != S_OK ){
		LogError("CoInitializeEx Failed");
	}
	if( CoCreateInstance(CLSID_SimServer, NULL,CLSCTX_LOCAL_SERVER, IID_IViewer, (void **)&pViewer) != S_OK ){
		LogError("CreateInstance Failed");
	}
	if( pViewer->QueryInterface(IID_ITeam, (void **)&pTeam) != S_OK ){
		LogError("QueryInterface ITeam Failed");
	}
	if( pTeam->TeamJoinIn(mySide, myColor, myType) != S_OK ){
		LogError("Team Join Failed");
	}
	LogInfo("SSLSimulator running.");
	
	if( pViewer->QueryInterface(IID_IController, (void **)&pController) != S_OK ){ // 控制server
		LogError("QueryInterface IController Failed");
	}
}
SSLSimulator::~SSLSimulator()
{
	if( pController ){
		pController->Release();
		pController = 0;
	}
	if( pPlayer ){
		pPlayer->Release();
		pPlayer = 0;
	}
	if( pTeam ){
		pTeam->Release();
		pTeam = 0;
	}
	if( pViewer ){
		pViewer->Release();
		pViewer = 0;
	}
	CoUninitialize();
}

bool SSLSimulator::sendCommand(int num, short type, short value1, short value2, short value3)
{
	if( type == CTStop || type == CTSpeed && value1 == 0 && value2 == 0 && value3 == 0 ){
		// 是停止命令
		if( stopCommandSent[num-1] ){
			return true;
		}else{
			stopCommandSent[num-1] = true;
		}
	}else{
		stopCommandSent[num-1] = false;
	}
	if( pTeam ){
		return (pTeam->TeamAct(num, type, value1, value2, value3) == S_OK);
	}
	if( pPlayer ){
		return (pPlayer->Act(type, value1, value2, value3) == S_OK);
	}
	return false;
}
bool SSLSimulator::getVisualInfo(VisualInfoT* pinfo)
{
	return (pViewer->GetVisualInfo(pinfo) == S_OK);
}
bool SSLSimulator::Pause(int status)
{
	return (pController->Pause(status) == S_OK);
}
bool SSLSimulator::MoveBall(double x, double y)
{
	return (pController->MoveBall(x,y,0,0) == S_OK);
}

void SSLSimulator::run()
{
	///> 接受图像周期
	static unsigned int cycle = 0;
	// ?? TODO 这种方法稳定帧率比较奇怪 by zhyaic 2017/06/04
#if defined(RBK_SYS_WINDOWS) && _MSC_VER <= 1600
	boost::chrono::time_point<boost::chrono::steady_clock> tp1;
	boost::chrono::time_point<boost::chrono::steady_clock> tp2;
	tp1 = boost::chrono::steady_clock::now();
#else
	std::chrono::time_point<std::chrono::steady_clock> tp1;
	std::chrono::time_point<std::chrono::steady_clock> tp2;
	tp1 = std::chrono::steady_clock::now();
#endif
	///> 图像信息接受，需要锁的配合，并进入循环
	while (true) {
		VisualInfo temp;
		
		if (!getVisualInfo(&temp)) {
			break;
		}
		//std::cout<<temp.ball.x<<std::endl;
		temp.cycle = ++cycle;	//// 暂时这么代替，fix me!!! 没有用到图像里面的cycle,而是策略自己记录cycle,收到一个图像便加1;
		sendRawVision(temp);
		// 两者图像接受间隔时间检测
#if defined(RBK_SYS_WINDOWS) && _MSC_VER <= 1600
		tp2 = boost::chrono::steady_clock::now();
		auto delt_time = boost::chrono::duration_cast<boost::chrono::milliseconds>((tp2 - tp1)).count();
		tp1 = boost::chrono::steady_clock::now();
#else
		tp2 = std::chrono::steady_clock::now();
		auto delt_time = std::chrono::duration_cast<std::chrono::milliseconds>((tp2 - tp1)).count();
		tp1 = std::chrono::steady_clock::now();
#endif
		// TODO
		//if (_log->m_bIsLogging) {
		//	_log->writeLog(_log->m_pfLog,temp);			
		//}
		//LogInfo(delt_time);
		if (delt_time < 1000.0/60) {
			// 睡眠，去掉噪声图像，保证稳定的图像帧率
			Sleep(1000.0/60 - delt_time);	
			//std::cout<<cycle<<'\t'<<1000.0/Param::Vision::FRAME_RATE-delt_time<<std::endl;
		}
		recCommandAndSend();
	}
}

void SSLSimulator::loadFromConfigFile()
{
	
}

void SSLSimulator::setSubscriberCallBack()
{
// 	setTopicCallBack<rbk::protocol::Message_NavSpeed>(&SSLSimulator::messageNavSpeedCallBack,this);
 	createPublisher<rbk::protocol::SRC_RawVision>();
	setTopicCallBack<rbk::protocol::SRC_Cmd>();
}

void SSLSimulator::messageNavSpeedCallBack(google::protobuf::Message* msg){
// 	rbk::protocol::Message_NavSpeed s;
// 	s.CopyFrom(*msg);
// 	sendCommand(1,);
// 	my_robot.setVel(s.x()*1000);				// mm/s
// 	my_robot.setRotVel(s.rotate()*180/3.1415926);		// deg/s
// 	publishTopic(s);
// 	return ;
}
namespace{
	enum CommandTypeT{ CTStop, CTDash, CTTurn, CTArc, CTSpeed, CTKick };
	inline int merge_command_type(int type, unsigned char dribble){
		if( dribble > 0){
			type |= 0x08;
		}
		return type;
	}
	void sendRawVision(const SSLSimulator::VisualInfo& info){
		rbk::protocol::SRC_RawVision vision;
		vision.set_frame_number(info.cycle);
		rbk::protocol::Ball* ball = vision.mutable_ball();
		ball->set_x(infoX2msgX(info.ball.x));
		ball->set_y(infoY2msgY(info.ball.y));
		ball->set_ball_found(info.ball.valid);
		ball->set_image_x(info.imageBall.x);
		ball->set_image_y(info.imageBall.y);
		ball->set_image_valid(info.imageBall.valid);
		if (isYellowTeam) {
			for(int i=0;i<6;i++){
				rbk::protocol::Robot* robot = vision.add_robots_yellow();
				robot->set_robot_id(i);
				robot->set_robot_found(info.player[i].pos.valid);
				robot->set_x(infoX2msgX(info.player[i].pos.x));
				robot->set_y(infoY2msgY(info.player[i].pos.y));
				robot->set_orientation(infoAngle2msgAngle(info.player[i].angle));
			}
			for(int i=6;i<12;i++){
				rbk::protocol::Robot* robot = vision.add_robots_blue();
				robot->set_robot_id(i-6);
				robot->set_robot_found(info.player[i].pos.valid);
				robot->set_x(infoX2msgX(info.player[i].pos.x));
				robot->set_y(infoY2msgY(info.player[i].pos.y));
				robot->set_orientation(infoAngle2msgAngle(info.player[i].angle));
			}
		} else{
			for(int i=0;i<6;i++){
				rbk::protocol::Robot* robot = vision.add_robots_blue();
				robot->set_robot_id(i);
				robot->set_robot_found(info.player[i].pos.valid);
				robot->set_x(infoX2msgX(info.player[i].pos.x));
				robot->set_y(infoY2msgY(info.player[i].pos.y));
				robot->set_orientation(infoAngle2msgAngle(info.player[i].angle));
			}
			for(int i=6;i<12;i++){
				rbk::protocol::Robot* robot = vision.add_robots_yellow();
				robot->set_robot_id(i-6);
				robot->set_robot_found(info.player[i].pos.valid);
				robot->set_x(infoX2msgX(info.player[i].pos.x));
				robot->set_y(infoY2msgY(info.player[i].pos.y));
				robot->set_orientation(infoAngle2msgAngle(info.player[i].angle));
			}
		}
		
		GRBKHandle::Instance()->publishTopic(vision);
	}
	void recCommandAndSend(){
		rbk::protocol::SRC_Cmd cmds;
		GRBKHandle::Instance()->getSubscriberData(cmds);
		for(int i=0;i<cmds.command_size();i++){
			rbk::protocol::Message_SSL_Command cmd = cmds.command(i);
			int id = cmd.robot_id();
			float velX = cmd.velocity_x() * 100;
			float velY = cmd.velocity_y() * 100;
			float velR = velTrans(cmd.velocity_r());
			float flat = cmd.flat_kick() * 100;
			float chip = cmd.chip_kick() * 100;
			int drib = cmd.dribbler_spin() > 0.01 ? 1 : 0;
			GRBKHandle::Instance()->sendCommand(id,merge_command_type(CTSpeed,drib),velX,velY,velR);
			GRBKHandle::Instance()->sendCommand(id,merge_command_type(CTKick, drib),flat,chip,0);// what is pass? ::::: black question mark.
		}
	}
}