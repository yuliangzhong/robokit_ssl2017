#include "RobotFollow.h"
#include "GlobalData.h"
#include "message.h"
#include "munkres.h"
#include "math.h"
#include <iostream>
using namespace std;
RobotFollow::RobotFollow(void)
{

}

RobotFollow::RobotFollow(int side,int cameraid1,int cameraid2)
{
    _side = side;
	_camera1=cameraid1;_camera2=cameraid2;
	initFollowArray();
	initSmsg();
	_matrix = Matrix<double>(MaxFollowNum,MaxFollowNum);
	
	for(int i = 0; i < SendCarNum; i++){
		existNum[i] = MaxFollowNum;
	}
}

RobotFollow::~RobotFollow(void)
{

}

void RobotFollow::initSmsg()
{
	for(int i = 0; i < SendCarNum; i++){
        GlobalData::instance()->smsg.RobotPosX[_side][i] = -ERRORNUM;
        GlobalData::instance()->smsg.RobotPosY[_side][i] = -ERRORNUM;
        GlobalData::instance()->smsg.RobotINDEX[_side][i] = MaxFollowNum;
        GlobalData::instance()->smsg.RobotFound[_side][i] = false;
        GlobalData::instance()->smsg.RobotRotation[_side][i] = -ERRORNUM;
	}
}

bool RobotFollow::determinesend(int minAddFrame, int minLostFrame, bool isSendFalse)
{
	for(int i = 0; i < MaxFollowNum; i++){
		if(followarray[i].Fresh == false){
			if(followarray[i].LostFrame < minLostFrame){
				followarray[i].LostFrame++;
			} else{
				followarray[i].LostFrame = minLostFrame;
				followarray[i].AddFrame = 0;
			}
		} else{
			if(followarray[i].AddFrame < minAddFrame){
				followarray[i].AddFrame++;
			} else{
				followarray[i].AddFrame = minAddFrame;
				followarray[i].LostFrame = 0;
			}
		}
		if(followarray[i].LostFrame == minLostFrame && followarray[i].AddFrame == 0){
			cancel(minLostFrame, i);
		}
	}

	for(int i = 0; i < SendCarNum; i++){
		int tmp = getFollowNum(existNum[i]);
		if(tmp >= 0){
			if(followarray[tmp].Fresh == true && followarray[tmp].AddFrame == minAddFrame) {
                GlobalData::instance()->smsg.RobotFound[_side][i] = true;
			} else {
				if (!isSendFalse) {
					if(followarray[tmp].LostFrame < minLostFrame && followarray[tmp].AddFrame == minAddFrame){
                        GlobalData::instance()->smsg.RobotFound[_side][i] = true;
					} else{
                        GlobalData::instance()->smsg.RobotFound[_side][i] = false;
					}
				} else{
                    GlobalData::instance()->smsg.RobotFound[_side][i] = followarray[tmp].Fresh;
				}
			}
            GlobalData::instance()->smsg.RobotINDEX[_side][i] = followarray[tmp].Num + 1;
            GlobalData::instance()->smsg.RobotPosX[_side][i] = followarray[tmp].PosX;
            GlobalData::instance()->smsg.RobotPosY[_side][i] = followarray[tmp].PosY;
            GlobalData::instance()->smsg.RobotRotation[_side][i] = followarray[tmp].Rotation*180/PI;
			followarray[tmp].Fresh = false;
		} else{
            GlobalData::instance()->smsg.RobotINDEX[_side][i] += 1;
		}
	}
	return false;
}

void RobotFollow::startFollowSingle(int court, int minAddFrame, int minLostFrame, float maxdist)
{
	// ??һ???볡?ĳ???????һ֡????ƥ??
	for(int row= 0; row< MaxFollowNum; row++){
		for(int col= 0; col< ReceiveCarNum; col++){
			if(distances(followarray[row].PosX,followarray[row].PosY,
                GlobalData::instance()->receiveMsg[court].RobotPosX[_side][col],
                GlobalData::instance()->receiveMsg[court].RobotPosY[_side][col]) < CALIERROR*CALIERROR){
					_matrix(row,col) = -ERRORNUM*ERRORNUM;
			} else{
				_matrix(row,col)=distances(followarray[row].PosX,followarray[row].PosY,
                    GlobalData::instance()->receiveMsg[court].RobotPosX[_side][col],
                    GlobalData::instance()->receiveMsg[court].RobotPosY[_side][col]);
			}
		}
	}

	Munkres _munkres;
	_munkres.solve(_matrix);
	for(int row = 0 ; row < MaxFollowNum; row++ ){
		for ( int col = 0 ; col <ReceiveCarNum; col++ ){
			if(_matrix(row,col) != INITNUM){
                if(/*isRecvValid(court, col) && */canBeSet(GlobalData::instance()->receiveMsg[court],row,col)
                    && GlobalData::instance()->receiveMsg[court].RobotFound[_side][col]){
                    followarray[row].PosX = GlobalData::instance()->receiveMsg[court].RobotPosX[_side][col];
                    followarray[row].PosY = GlobalData::instance()->receiveMsg[court].RobotPosY[_side][col];
                    followarray[row].Rotation = GlobalData::instance()->receiveMsg[court].RobotRotation[_side][col];
					followarray[row].Fresh = true;
                    if(GlobalData::instance()->smsg.Cycle % GlobalData::instance()->followCheckCycle == 0
                        && GlobalData::instance()->receiveMsg[court].RobotFound[_side][col]){
                            followarray[row].Num = GlobalData::instance()->receiveMsg[court].RobotINDEX[_side][col];
					}
				}
			}
		}
	}
}

void RobotFollow::startFollowDouble(int minAddFrame, int minLostFrame,float maxdist)
{
	ReceiveVisionMessage mixedrecMsg;
    for(int i = 0; i<MaxFollowNum; i++){
		mixedrecMsg.RobotFound[_side][i] = false;
		mixedrecMsg.RobotINDEX[_side][i] = MaxFollowNum;
		mixedrecMsg.RobotPosX[_side][i] = -ERRORNUM;
		mixedrecMsg.RobotPosY[_side][i] = -ERRORNUM;
		mixedrecMsg.RobotRotation[_side][i] = -ERRORNUM;
	}
	int countNum = 0;
	for(int leftnum=0;leftnum<MaxFollowNum;leftnum++){
        if(GlobalData::instance()->receiveMsg[_camera1].RobotFound[_side][leftnum]){
			int rightnum = 0;
			for (rightnum = 0; rightnum < MaxFollowNum; rightnum++){
				if(ismix(leftnum,rightnum) && countNum < MaxFollowNum){
                    ReceiveVisionMessage tmpmsg=GlobalData::instance()->receiveMsg[_camera1];
                    ReceiveVisionMessage tmpmsg2=GlobalData::instance()->receiveMsg[_camera2];
					mixedrecMsg.RobotPosX[_side][countNum]=
                    (GlobalData::instance()->receiveMsg[_camera1].RobotPosX[_side][leftnum]+GlobalData::instance()->receiveMsg[_camera2].RobotPosX[_side][rightnum])/2;
					mixedrecMsg.RobotPosY[_side][countNum]=
                    (GlobalData::instance()->receiveMsg[_camera1].RobotPosY[_side][leftnum]+GlobalData::instance()->receiveMsg[_camera2].RobotPosY[_side][rightnum])/2;
                    double tmp1 = (cos(GlobalData::instance()->receiveMsg[_camera1].RobotRotation[_side][leftnum]) +
                        cos(GlobalData::instance()->receiveMsg[_camera2].RobotRotation[_side][rightnum])) / 2;
                    double tmp2 = (sin(GlobalData::instance()->receiveMsg[_camera1].RobotRotation[_side][leftnum]) +
                        sin(GlobalData::instance()->receiveMsg[_camera2].RobotRotation[_side][rightnum])) / 2;
					mixedrecMsg.RobotRotation[_side][countNum]=atan2f(tmp2,tmp1);
					mixedrecMsg.RobotFound[_side][countNum]=true;
                    mixedrecMsg.RobotINDEX[_side][countNum]=GlobalData::instance()->receiveMsg[_camera1].RobotINDEX[_side][leftnum];
					countNum++;
					break;
				}
			}
			if(rightnum == MaxFollowNum && countNum < MaxFollowNum && isRecvValid(_camera1,leftnum)){
                ReceiveVisionMessage temdrecMsg = GlobalData::instance()->receiveMsg[_camera1];
				mixedrecMsg.RobotFound[_side][countNum]=true;
                mixedrecMsg.RobotINDEX[_side][countNum]=GlobalData::instance()->receiveMsg[_camera1].RobotINDEX[_side][leftnum];
                mixedrecMsg.RobotPosX[_side][countNum]=GlobalData::instance()->receiveMsg[_camera1].RobotPosX[_side][leftnum];
                mixedrecMsg.RobotPosY[_side][countNum]=GlobalData::instance()->receiveMsg[_camera1].RobotPosY[_side][leftnum];
                mixedrecMsg.RobotRotation[_side][countNum]=GlobalData::instance()->receiveMsg[_camera1].RobotRotation[_side][leftnum];
				countNum++;
			}
		}
	}
	for(int rightnum = 0; rightnum < MaxFollowNum; rightnum++){
        if(GlobalData::instance()->receiveMsg[_camera2].RobotFound[_side][rightnum]){
			int leftnum = 0;
			for (leftnum = 0; leftnum < MaxFollowNum; leftnum++){
				if(ismix(leftnum,rightnum) && countNum < MaxFollowNum){
					break;
				}
			}
			if(leftnum == MaxFollowNum && countNum < MaxFollowNum && isRecvValid(_camera2,rightnum)){
				mixedrecMsg.RobotFound[_side][countNum]=true;
                mixedrecMsg.RobotINDEX[_side][countNum]=GlobalData::instance()->receiveMsg[_camera2].RobotINDEX[_side][rightnum];
                mixedrecMsg.RobotPosX[_side][countNum]=GlobalData::instance()->receiveMsg[_camera2].RobotPosX[_side][rightnum];
                mixedrecMsg.RobotPosY[_side][countNum]=GlobalData::instance()->receiveMsg[_camera2].RobotPosY[_side][rightnum];
                mixedrecMsg.RobotRotation[_side][countNum]=GlobalData::instance()->receiveMsg[_camera2].RobotRotation[_side][rightnum];
				countNum++;
			}
		}
	}

	//ƥ??
	for(int row= 0; row< MaxFollowNum; row++){
		for(int col= 0; col< MaxFollowNum; col++){
			if(	distances(followarray[row].PosX,followarray[row].PosY,
				mixedrecMsg.RobotPosX[_side][col],mixedrecMsg.RobotPosY[_side][col]) < CALIERROR*CALIERROR){
				_matrix(row,col)= -ERRORNUM*ERRORNUM;
			} else{
				_matrix(row,col)=sqrt(distances(followarray[row].PosX,followarray[row].PosY,mixedrecMsg.RobotPosX
				[_side][col],mixedrecMsg.RobotPosY[_side][col]));
			}
		}
	}

	Munkres _munkres;
	_munkres.solve(_matrix);

	//??ֵ
	for(int row= 0; row< MaxFollowNum; row++){
		for(int col= 0; col< MaxFollowNum; col++){
			if(_matrix(row,col) != INITNUM){
				if(mixedrecMsg.RobotFound[_side][col] && canBeSet(mixedrecMsg, row, col)){
					followarray[row].Fresh=true;
					followarray[row].PosX = mixedrecMsg.RobotPosX[_side][col];
					followarray[row].PosY = mixedrecMsg.RobotPosY[_side][col];
					followarray[row].Rotation = mixedrecMsg.RobotRotation[_side][col];
                    if(GlobalData::instance()->smsg.Cycle % GlobalData::instance()->followCheckCycle == 0
						&& mixedrecMsg.RobotFound[_side][col]){
							followarray[row].Num = mixedrecMsg.RobotINDEX[_side][col];
					}
				}
			}
		}
	}
}

bool RobotFollow::ismix(int leftnum,int rightnum)
{
	if (isRecvValid(_camera1,leftnum)
		&&isRecvValid(_camera2,rightnum)
        &&distances(GlobalData::instance()->receiveMsg[_camera1].RobotPosX[_side][leftnum],
          GlobalData::instance()->receiveMsg[_camera1].RobotPosY[_side][leftnum],
          GlobalData::instance()->receiveMsg[_camera2].RobotPosX[_side][rightnum],
          GlobalData::instance()->receiveMsg[_camera2].RobotPosY[_side][rightnum])<CALIERROR*CALIERROR)
	{
		return true;
	}
	return false;
} 

bool RobotFollow::canBeSet(ReceiveVisionMessage msg, int row, int col)
{
	float preX = followarray[row].PosX;
	float preY = followarray[row].PosY;
	float nowX = msg.RobotPosX[_side][col];
	float nowY = msg.RobotPosY[_side][col];
	float nowDir = msg.RobotRotation[_side][col];
	int nowIndex = msg.RobotINDEX[_side][col];
	if(isPosValid(preX, preY) && isPosValid(nowX, nowY)){
		if(distances(preX, preY, nowX, nowY) < CALIERROR*CALIERROR){
			return true;
		} else{
			addFollow(nowX, nowY, nowDir, nowIndex);
			return false;
		}
	} else if(isPosValid(preX, preY) && !isPosValid(nowX, nowY)){
		return false;
	}
	return true;

}

void RobotFollow::addFollow(float x, float y, float dir, int index)
{
	for(int i = 0; i<MaxFollowNum; i++){
		if(followarray[i].PosX == - ERRORNUM && followarray[i].PosY == - ERRORNUM){
			followarray[i].PosX = x;
			followarray[i].PosY = y;
			followarray[i].Rotation = dir;
			followarray[i].Num = index;
			followarray[i].Fresh = true;
			break;
		}
	}
}

bool RobotFollow::isRecvValid(int court, int col)
{
    if(GlobalData::instance()->receiveMsg[court].RobotINDEX[_side][col]>=0
        && GlobalData::instance()->receiveMsg[court].RobotINDEX[_side][col]<12
        && GlobalData::instance()->receiveMsg[court].RobotPosY[_side][col]>-MaxValidWidth
        && GlobalData::instance()->receiveMsg[court].RobotPosY[_side][col]<MaxValidWidth
        && GlobalData::instance()->receiveMsg[court].RobotPosX[_side][col]>-MaxValidLength
        && GlobalData::instance()->receiveMsg[court].RobotPosX[_side][col]<MaxValidLength
        && GlobalData::instance()->receiveMsg[court].RobotFound[_side][col]){
			return true;
	}
	return false;
}

bool RobotFollow::isPosValid(float x, float y)
{
	if( x>-MaxValidLength && x<MaxValidLength && y>-MaxValidWidth && y<MaxValidWidth)
		return true;
	return false;
}

void RobotFollow::compare(int minAddFrame)
{
	int pExist[ReceiveCarNum];
	int pLen = 0;// the length of array pExist

	// ??existNum????ͬ?ĳ??źϲ???һ??  add by zhyaic 2012.5.30
	for(int i=0; i<SendCarNum; i++){
		for(int j=i+1; j<SendCarNum; j++){
			if(existNum[i] == existNum[j] && existNum[i] != ReceiveCarNum){
				existNum[j] = ReceiveCarNum;
			}
		}
	}

	int Join[ReceiveCarNum];
	for(int i=0; i<ReceiveCarNum; i++){
		Join[i]=0;
	}

	for(int i=0; i<ReceiveCarNum; i++){
		pExist[i] = -1;
	}

	
	for(int i = 0; i < MaxFollowNum; i++ ){
		if( followarray[i].AddFrame == minAddFrame && followarray[i].Num != ReceiveCarNum){
			pExist[pLen] = followarray[i].Num;
			pLen++;
		}
	}
	
	for(int i = 0; i < pLen; i++){
		if(pExist[i]>=0 && pExist[i] < ReceiveCarNum){
			Join[pExist[i]]++;
		}
	}
	
	for(int i = 0; i < SendCarNum; i++){
		if(existNum[i] >=0 && existNum[i] < ReceiveCarNum)
			Join[existNum[i]]++;
	}

	for(int i = 0; i < pLen; i++) {// check new set and old set
		if(pExist[i] >= 0 && pExist[i] < ReceiveCarNum) {		
			int flag=0;
			int signs;
			// ???????????ڵĳ??ϸ????ھ??У???ôֱ??break
			for(int j = 0; j < SendCarNum; j++) {
				if(pExist[i] == existNum[j]){
					flag=1;
					break;
				}
			}
			if(!flag){
				// ?????ϸ????ڴ??ڵĳ?
				for(int k = 0; k < SendCarNum; k++) {
					int tmp = existNum[k];
					// ?????п?λ??ֱ?ӷ??????????ڵ?һ????
					if(tmp == ReceiveCarNum){
						existNum[k] = pExist[i];
						Join[pExist[i]] = 2;
						break;
					} 
					// ?ϸ??????еĳ???????????û????,???³???ֱ?Ӷ???
					else if (Join[tmp] == 1 && tmp>=0 && tmp < ReceiveCarNum && pExist[i] != tmp){
						existNum[k] = pExist[i];
						Join[pExist[i]] = 2;
						break;
					} 
					// ???仰??????????????????
					else if(Join[tmp]==1) {
						existNum[k] = ReceiveCarNum;
					}
					/*if((Join[tmp] == 1 || tmp == ReceiveCarNum) && pExist[i] != tmp){
						existNum[k]=pExist[i];
						Join[pExist[i]]++;
						break;
					} else if(Join[tmp]==1) {
						existNum[k] = ReceiveCarNum;
					}*/
				}
			}
		}
	}
}

void RobotFollow::Usort()
{
	for(int i = 0; i < SendCarNum-1; i++){
		int minimum=existNum[i];
		int signs=i;
		for(int j = i+1; j < SendCarNum; j++){
			if(minimum > existNum[j]){
				minimum=existNum[j];
				signs=j;
			}
		}
		int tmp;
		tmp = existNum[i];
		existNum[i] = existNum[signs];
		existNum[signs] = tmp;
	}
}
