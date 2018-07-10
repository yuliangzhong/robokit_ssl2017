#include "RobotSensor.h"
#include <PlayInterface.h>
#include "BestPlayer.h"
#include <WorldModel.h>
#include <fstream>
#include "Global.h"
#include "src_cmd.pb.h"
#include "SSLStrategy.h"

using namespace std;

extern bool IS_SIMULATION;

namespace {	
	//ofstream stopmsgfile("D:\\stopmsg.txt");
	bool debug = true;
}

CRobotSensor::CRobotSensor()
{
	// 数据进行初始化
	memset(_isValid, false, sizeof(_isValid));
	memset(_lastKickingChecked, false, sizeof(_lastKickingChecked));
	memset(_lastCheckedKickingCycle, 0, sizeof(_lastCheckedKickingCycle));	

	for(int i = 0; i < Param::Field::MAX_PLAYER+1; i ++) {
		robotInfoBuffer[i].bInfraredInfo = false;
		robotInfoBuffer[i].nKickInfo = 0;

		_lastInfraredInfo[i] = false;
		_last_change_num[i] = -1;
		_last_real_num_index[i] = -1;
	}
}

void CRobotSensor::Update(int cycle)
{
	// 仿真不处理，直接予以返回
	if (IS_SIMULATION) {
		return;
	}

	rbk::protocol::Robots_Status robots_status;
	GRBKHandle::Instance()->getSubscriberData(robots_status);
	if (robots_status.robots_status_size() > 0) {
		//std::cout<<robots_status.DebugString()<<std::endl;
	}
	for (int i = 1; i <= Param::Field::MAX_PLAYER; i ++) {
		int realNum = PlayInterface::Instance()->getRealIndexByNum(i);
		if (realNum >= 1&& realNum <= 12 && robots_status.robots_status_size() > 0) {
			rbk::protocol::Robot_Status rs = robots_status.robots_status(realNum - 1);
			if (rs.robot() != realNum) {
				LogError("Do not match real number: "<< rs.robot()<<" "<<realNum);
			} else{
				rawDataBuffer.bInfraredInfo = rs.infrared();
				rawDataBuffer.nRobotNum = rs.robot();
				rawDataBuffer.nKickInfo = rs.chip_kick() || rs.flat_kick();
			}

			// 强制保持一段时间球被踢出
			if (_lastKickingChecked[i] || robotInfoBuffer[i].nKickInfo > 0) {
				if (cycle - _lastCheckedKickingCycle[i] > 5) {
					_lastKickingChecked[i] = false;
					robotInfoBuffer[i].nKickInfo = 0;
				}
			}

			if (rs.change_num() != _last_change_num[i] || rs.robot() != _last_real_num_index[i]) {
				_last_change_num[i] = rs.change_num();
				_last_real_num_index[i] = rs.robot();
				//std::cout<<rs.infrared()<<" "<<rs.flat_kick()<<" "<<rs.chip_kick()<<std::endl;
				_isValid[i] = true;

				UpdateBallDetected(i);

				// 表明是踢球的包
				if (rawDataBuffer.nKickInfo) {
					robotInfoBuffer[i] = rawDataBuffer;
					_lastKickingChecked[i] = true;
					_lastCheckedKickingCycle[i] = cycle;
				}

			} else {
				_lastKickingChecked[i] = false;
			}
		}
	}
}

bool CRobotSensor::IsInfraredOn(int num)				
{ 
	if(IS_SIMULATION){
		return BestPlayer::Instance()->isOurPlayerStrictControlBall(num);
	} else{
		return robotInfoBuffer[num].bInfraredInfo; 
	}
}

void CRobotSensor::UpdateBallDetected(int num)
{
	// 丢掉除了自己本身的数据
	int realnum = PlayInterface::Instance()->getRealIndexByNum(num);
	if (realnum != rawDataBuffer.nRobotNum) {
		return ;
	}

	// 现在红外信号时改变才上传 [8/7/2011 cliffyin]
	// 红外
	bool currentInfraredInfo = rawDataBuffer.bInfraredInfo;
	bool InfraredInfoReset = false;
	if (_lastInfraredInfo[num] != currentInfraredInfo) {
		InfraredInfoReset = true;
		_lastInfraredInfo[num] = currentInfraredInfo;
	}

	if (InfraredInfoReset) {
		robotInfoBuffer[num].bInfraredInfo = currentInfraredInfo;
	}

	return ;
}