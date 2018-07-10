#ifndef _COMPENSATE_
#define _COMPENSATE_

//用于计算一传一射时的角度补偿
#include <geometry.h>
#include "singleton.h"

class CCompensate{
public:
	CCompensate(); 
	~CCompensate();
	double checkCompensate(int playerNum,double ballspeed,double rawdir);
	double getKickDir(int playerNum, CGeoPoint kickTarget);
	void setOurRobotIndex(unsigned char*_ourRobotIndex);
private:
	void readCompensateTable();
	double compensatevalue[15][100][50];
	unsigned char ourRobotIndex[6];
};

typedef NormalSingleton< CCompensate > Compensate;

#endif 