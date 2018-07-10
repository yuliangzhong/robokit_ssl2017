#ifndef _ROBOT_FILTERED_INFO_H_
#define _ROBOT_FILTERED_INFO_H_

#include <singleton.h>
#include <param.h>
#include "WorldDefine.h"
/*
* 小车本体感知信息接收处理 created by qxz
*/

/// 预定义的一些常量宏
#define MAX_STORE_SIZE 5  // 保留最近5帧的信息
#define NO_KICK 0
#define IS_FLAT_KICK_ON 1
#define IS_CHIP_KICK_ON 2
#define LOWEST_ENERGY 0
#define LOW_ENERGY 1
#define NORMAL_ENERGY 2
#define HIGH_ENERGY 3
#define HIGHEST_ENERGY 4

/// 机器人本体感知信息
class CRobotSensor{
public:
	// 构造函数
	CRobotSensor();
	// 析构函数
	~CRobotSensor() {}

	// 全局更新借口
	void Update(int cycle);

	// 小车双向通讯是否已经启动，数据是否是最新的
	bool IsInfoValid(int num)				{ return _isValid[num]; }

	// 小车是否产生红外信号，一般表示球在嘴前方
	bool IsInfraredOn(int num);
	// 重置小车的红外信号
	void ResetInraredOn(int num)			{ _lastInfraredInfo[num] = false;  robotInfoBuffer[num].bInfraredInfo = false; }

	// 小车有否启动平射或挑射的机构
	int IsKickerOn(int num)					{ return robotInfoBuffer[num].nKickInfo; }

private:
	// 更新的内部接口
	void UpdateBallDetected(int num);

	ROBOTINFO robotInfoBuffer[Param::Field::MAX_PLAYER+1];		// 存放过滤后的结果
	ROBOTINFO rawDataBuffer;

	int _last_real_num_index[Param::Field::MAX_PLAYER+1];
	int _last_change_num[Param::Field::MAX_PLAYER+1];

	bool _lastKickingChecked[Param::Field::MAX_PLAYER+1];
	int _lastCheckedKickingCycle[Param::Field::MAX_PLAYER+1];
                         
	bool _isValid[Param::Field::MAX_PLAYER+1];					// 当前数据是否是最新的

	bool _lastInfraredInfo[Param::Field::MAX_PLAYER+1];			// 历史的红外数据
};

typedef NormalSingleton< CRobotSensor > RobotSensor;			// 全局访问接口
#endif