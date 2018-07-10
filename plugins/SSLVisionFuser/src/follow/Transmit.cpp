#include "Transmit.h"
#include "GlobalData.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
using namespace std;
namespace{
	int sortCount = 0;
}

Transmit::Transmit(){
	initRmsgArray();

    for (int i=0;i<GlobalData::instance()->cameraNum/2;i++)
	{
		if(i==0)
		{
        _cameraMixer[0]=TwoCameraMixer(2,3);
		}
		
		else if(i==1)
		{
        _cameraMixer[1]=TwoCameraMixer(0,1);
		}
	}
	_lastCameraMixer=TwoCameraMixer(MaxNumOfCamera,MaxNumOfCamera+1);
}
Transmit::~Transmit(){
}
void Transmit::setFirstStart()
{ 
    GlobalData::instance()->firstStart = true;
	sortCount = 0; 
}
void Transmit::usortCounter()
{
    if (GlobalData::instance()->firstStart)
	{
		sortCount++;
		if(sortCount == 60){
            GlobalData::instance()->firstStart = false;
			sortCount = 0;
		}
	}
}
void Transmit::setCycle(int followcycle, int cycle)
{
     GlobalData::instance()->followCheckCycle = followcycle;
     GlobalData::instance()->smsg.Cycle = cycle;
}

void Transmit::initRmsgArray()
{
	
	for(int i = 0; i < MaxNumOfCamera+2 ; i++){
		for (int j = 0; j < MaxBallNum; j++) {
			GlobalData::instance()->receiveMsg[i].Ballx[j] = -ERRORNUM;
			GlobalData::instance()->receiveMsg[i].Bally[j] = -ERRORNUM;
			GlobalData::instance()->receiveMsg[i].BallFound[j] = false;
		}
        for (int j = 0; j < ReceiveCarNum; j++) {
            GlobalData::instance()->receiveMsg[i].RobotINDEX[BLUE][j] = MaxFollowNum;
            GlobalData::instance()->receiveMsg[i].RobotFound[BLUE][j] = false;
            GlobalData::instance()->receiveMsg[i].RobotPosX[BLUE][j] = -ERRORNUM;
            GlobalData::instance()->receiveMsg[i].RobotPosY[BLUE][j] = -ERRORNUM;
            GlobalData::instance()->receiveMsg[i].RobotRotation[BLUE][j] = -ERRORNUM;
            GlobalData::instance()->receiveMsg[i].RobotINDEX[YELLOW][j] = MaxFollowNum;
            GlobalData::instance()->receiveMsg[i].RobotFound[YELLOW][j] = false;
            GlobalData::instance()->receiveMsg[i].RobotPosX[YELLOW][j] = -ERRORNUM;
            GlobalData::instance()->receiveMsg[i].RobotPosY[YELLOW][j] = -ERRORNUM;
            GlobalData::instance()->receiveMsg[i].RobotRotation[YELLOW][j] = -ERRORNUM;
		}
	}
}

SendVisionMessage Transmit::smsgUpdate(int cameraMode,int minAddFrame, int minLostFrame,float maxdist,bool isFalseSend)
{
    usortCounter();
	int startNum=0;
	if (cameraMode==TwoCamDown||cameraMode==SingleCamLeftDown||cameraMode==SingleCamRightDown)
		startNum=1;

    for (int i=startNum;i<GlobalData::instance()->cameraNum/2+startNum;i++) {
        _cameraMixer[i].mixAlgorithm(cameraMode,1,1,maxdist,isFalseSend);
        GlobalData::instance()->changeSmsgToRmsg(i+MaxNumOfCamera);
    }
    if (cameraMode==FourCamera) {
		_lastCameraMixer.mixAlgorithm(cameraMode,minAddFrame,minLostFrame,maxdist,isFalseSend);
	}

	initRmsgArray();
    return GlobalData::instance()->smsg;
}
//void Transmit::process() {
//    emit finished();
//}
