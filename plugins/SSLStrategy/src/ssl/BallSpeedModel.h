#ifndef _BALL_SPEED_MODEL_H_
#define _BALL_SPEED_MODEL_H_

#include "VisionModule.h"

class CBallSpeedModel{
public:
	CBallSpeedModel();
	CVector speedForTime(int frame, const CVisionModule* pVision);   //��������֡�Ժ���ٶ�
	CVector speedForDist(double dist, const CVisionModule* pVision); //�������ɾ������ٶ�
	double timeForDist(double dist, const CVisionModule* pVision);      //�������˶����ɾ����ʱ��
	CGeoPoint posForTime(int frame, const CVisionModule* pVision);   //��������֡�Ժ�ľ���λ��
	void update( const CVisionModule* pVision );
	double CalKickInitSpeed(const double dist);
private:
	CVector _ballVel;
	CVector _lastBallVel;
	CGeoPoint _ballPos;
	double cal_acc;
	double _last_dist;
};
typedef NormalSingleton< CBallSpeedModel > BallSpeedModel;
#endif // _BALL_SPEED_MODEL_H_

