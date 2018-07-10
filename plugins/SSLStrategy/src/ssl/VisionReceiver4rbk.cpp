#include "VisionReceiver4rbk.h"
#include "SSLStrategy.h"
#include "referee.pb.h"
#include "src_rawvision.pb.h"
#include "utils.h"
#include "OptionModule.h"
#include "game_state.h"
#include <tinyxml/ParamReader.h>
#include "VisionLog.h"
namespace {
	struct sCMD_TYPE {
		char cmd;
		unsigned int step;
	};
	struct stGamePacket {
		char cmd;
		unsigned char cmd_counter;
		unsigned char goals_blue;
		unsigned char goals_yellow;
		unsigned short time_remaining;
	};
	float msgX2InfoX(float x) { return (x/10.0); }
	float msgY2InfoY(float y) { return (-y/10.0); }
	float msgAngle2InfoAngle(float angle) { return -Utils::Deg2Rad(angle); }
	bool WRITE_LOG = false;
	VisionLog *_log;
}
CVisionReceiver4rbk::CVisionReceiver4rbk() {
	{
		DECLARE_PARAM_READER_BEGIN(General)
			READ_PARAM(WRITE_LOG)
		DECLARE_PARAM_READER_END
	}
	_log = new VisionLog;
		if (WRITE_LOG){
		_log->createFile();
		std::cout << "Log create"<< std::endl;
		_log->m_bIsLogging = true;
	}
	m_play_mode = PMNone;
}

CVisionReceiver4rbk::~CVisionReceiver4rbk(void) {
	if(_log && _log->m_bIsLogging)
	{
		std::cout << "Deleting Log" << std::endl;
		_log->delEmptyFile();//删除无效Log
		delete _log;
	}
}
bool CVisionReceiver4rbk::rawVision2VisualInfo(const COptionModule *pOption,VisualInfoT& info){
	static int strategy_cycle = 0;
	rbk::protocol::SRC_RawVision vision;
	GRBKHandle::Instance()->getSubscriberData(vision);
	info.cycle = ++strategy_cycle;
	info.mode = m_play_mode;
	info.ball.valid = vision.ball().ball_found();
	info.ball.x = msgX2InfoX(vision.ball().x());
	info.ball.y = msgY2InfoY(vision.ball().y());
	info.imageBall.valid = vision.ball().image_valid();
	info.imageBall.x = vision.ball().image_x();
	info.imageBall.y = vision.ball().image_y();
	if (pOption->MyColor() == TEAM_BLUE){
		for(int i=0;i<vision.robots_blue_size();i++){
			const rbk::protocol::Robot& robot = vision.robots_blue(i);
			info.player[i].angle = msgAngle2InfoAngle(robot.orientation());
			info.player[i].pos.valid = robot.robot_found();
			info.player[i].pos.x = msgX2InfoX(robot.x());
			info.player[i].pos.y = msgY2InfoY(robot.y());
			info.ourRobotIndex[i] = robot.robot_id();
		}
		for(int i=Param::Field::MAX_PLAYER;i<Param::Field::MAX_PLAYER+vision.robots_yellow_size();i++){
			const rbk::protocol::Robot& robot = vision.robots_yellow(i-Param::Field::MAX_PLAYER);
			info.player[i].angle = msgAngle2InfoAngle(robot.orientation());
			info.player[i].pos.valid = robot.robot_found();
			info.player[i].pos.x = msgX2InfoX(robot.x());
			info.player[i].pos.y = msgY2InfoY(robot.y());
			info.theirRobotIndex[i-Param::Field::MAX_PLAYER] = robot.robot_id();
		}
	}else{
		for(int i=0;i<vision.robots_yellow_size();i++){
			const rbk::protocol::Robot& robot = vision.robots_yellow(i);
			info.player[i].angle = msgAngle2InfoAngle(robot.orientation());
			info.player[i].pos.valid = robot.robot_found();
			info.player[i].pos.x = msgX2InfoX(robot.x());
			info.player[i].pos.y = msgY2InfoY(robot.y());
			info.ourRobotIndex[i] = robot.robot_id();
		}
		for(int i=Param::Field::MAX_PLAYER;i<Param::Field::MAX_PLAYER+vision.robots_blue_size();i++){
			const rbk::protocol::Robot& robot = vision.robots_blue(i-Param::Field::MAX_PLAYER);
			info.player[i].angle = msgAngle2InfoAngle(robot.orientation());
			info.player[i].pos.valid = robot.robot_found();
			info.player[i].pos.x = msgX2InfoX(robot.x());
			info.player[i].pos.y = msgY2InfoY(robot.y());
			info.theirRobotIndex[i-Param::Field::MAX_PLAYER] = robot.robot_id();
		}
	}
	return true;
}

bool CVisionReceiver4rbk::getVisionInfo(const COptionModule *pOption,VisualInfoT& info, RefRecvMsg& msg){
	SSL_Referee ssl_referee;
	GRBKHandle::Instance()->getSubscriberData(ssl_referee);

	if (!rawVision2VisualInfo(pOption,info)){
		return false;
	}

	unsigned long long packet_timestamp = ssl_referee.packet_timestamp();
	SSL_Referee_Stage stage = ssl_referee.stage();
	SSL_Referee_Command command = ssl_referee.command();
	unsigned long command_counter = ssl_referee.command_counter();
	unsigned long long command_timestamp = ssl_referee.command_timestamp();
	SSL_Referee_TeamInfo yellow = ssl_referee.yellow();
	SSL_Referee_TeamInfo blue = ssl_referee.blue();
	long long stage_time_left = 0;
	stage_time_left = ssl_referee.stage_time_left();
	char cmd;
	struct stGamePacket pCmd_temp;
	unsigned char cmd_index = 0;
	switch(command) {
	case 0: cmd = 'H'; break; // Halt
	case 1: cmd = 'S'; break; // Stop
	case 2: cmd = ' '; break; // Normal start (Ready)
	case 3: cmd = 's'; break; // Force start (Start)
	case 4: cmd = 'k'; break; // Kickoff Yellow
	case 5: cmd = 'K'; break; // Kickoff Blue
	case 6: cmd = 'p'; break; // Penalty Yellow
	case 7: cmd = 'P'; break; // Penalty Blue
	case 8: cmd = 'f'; break; // Direct Yellow
	case 9: cmd = 'F'; break; // Direct Blue
	case 10: cmd = 'i'; break; // Indirect Yellow
	case 11: cmd = 'I'; break; // Indirect Blue
	case 12: cmd = 't'; break; // Timeout Yellow
	case 13: cmd = 'T'; break; // Timeout Blue
	case 14: cmd = 'g'; break; // Goal Yellow
	case 15: cmd = 'G'; break; // Goal Blue
	default: cmd = 'H';break;
	}
	pCmd_temp.cmd = cmd;
	pCmd_temp.cmd_counter = command_counter; 
	pCmd_temp.goals_blue = blue.score();
	pCmd_temp.goals_yellow = yellow.score();
	pCmd_temp.time_remaining = unsigned short(stage_time_left / 1000000);
	static unsigned char former_cmd_index = 0;
	cmd_index = pCmd_temp.cmd_counter;
	if (cmd_index != former_cmd_index) {
		former_cmd_index = cmd_index;	// 更新上一次指令得标志值
		m_play_mode = PMNone;
		for( int pm = PMStop; pm <= PMIndirectBlue; ++pm ) {
			if( playModePair[pm].ch == cmd ) {
				// 寻找匹配的指令名字
				m_play_mode = playModePair[pm].mode;
				break;
			}
		}
		if( m_play_mode != PMNone ) {
			info.mode = m_play_mode;
			msg.blueGoal = (int)pCmd_temp.goals_blue;
			msg.yellowGoal = (int)pCmd_temp.goals_yellow;
			msg.blueGoalie = (int)blue.goalie();
			msg.yellowGoalie = (int)yellow.goalie();
			msg.timeRemain = pCmd_temp.time_remaining;
			std::cout << "Protobuf Protocol: RefereeBox Command : " << cmd << " what : " << playModePair[m_play_mode].what << std::endl;
			//std::cout << "Stage_time_left : "<< msg.timeRemain << " Goals for blue : "<< (int)(pCmd_temp.goals_blue)
			//	<< " Goals for yellow : "<< (int)(pCmd_temp.goals_yellow) << std::endl;
		}
	}
	if (_log->m_bIsLogging) {
		_log->writeLog(_log->m_pfLog,info);			
	}
	return true;
}