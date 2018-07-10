#include "GDebugEngine.h"
#include "skill/Factory.h"
#include <utils.h>
#include "param.h"
#include <cmath>
#include <utils.h>
#include <VisionModule.h>
#include "BallStatus.h"
#include "ShootBall.h"
#include "TouchKickPos.h"
#include "KickDirection.h"
#include "WorldModel.h"
#include <KickStatus.h>
#include "RobotSensor.h"
#include "DribbleStatus.h"

namespace
{
	enum ShootBall_State{
		TOUCH_KICK = 1,
		CHASE_KICK,
		INTERCEPT_KICK,
		INTER_TO_TOUCH
	};
	bool verBos = false;
	int Touch2ChaseCnt = 0;
	int Touch2InterCnt = 0;
	int Chase2TouchCnt = 0;
	int Chase2InterCnt = 0;
	int MaxOutInterDribbleCycle = 60;
};

CShootBall::CShootBall()
{
	_lastCycle = 0;
	_out_inter_cnt = 0;
	_backShootDir=0;
	_out_inter = false;
}

// �������ĳ�����һ���Ƚ��ȶ��ĳ���������TouchKick�����Ƕ�
// ��Chase��Intercept��˵��Ҫʱʱ���㳯��Touch�Ǿ�̬��������
// Chase��Intercept�Ƕ�̬������������
void CShootBall::plan(const CVisionModule* pVision)
{
	const int runner = task().executor;
	int flags = task().player.flag;
	double finalDir = task().player.angle;
	const PlayerVisionT& self = pVision->OurPlayer(runner);
	const MobileVisionT& ball = pVision->Ball();
	const CGeoPoint rawBallPos=pVision->RawBall().Pos();
	CVector self2ball = rawBallPos - self.Pos();
	const double ballVelDir = ball.Vel().dir();
	double antiBallVelDir = Utils::Normalize(ballVelDir + Param::Math::PI);
	double ballSpeed = ball.Vel().mod();
	CGeoSegment shootSeg(self.Pos(), self.Pos() + Utils::Polar2Vector(1500, finalDir));
	CGeoPoint ballShootSegInterP = shootSeg.projection(rawBallPos);
	CGeoLine ballMovingLine = CGeoLine(rawBallPos,rawBallPos+Utils::Polar2Vector(1200,ball.Vel().dir()));
	CGeoPoint projMePos = ballMovingLine.projection(self.Pos());
	double me2ballMovingLineDist = projMePos.dist(self.Pos());


	const CGeoSegment ballMovingSeg = CGeoSegment(rawBallPos,rawBallPos+Utils::Polar2Vector(1200,Utils::Normalize(ballVelDir)));	
	const CGeoPoint projMe = ballMovingSeg.projection(self.Pos());					//С�������ƶ����ϵ�ͶӰ��
	double projDist = projMe.dist(self.Pos());										//С����ͶӰ��ľ���
	const double ball2projDist = projMe.dist(rawBallPos);							//ͶӰ�㵽��ľ���
	const bool meOnBallMovingSeg = ballMovingSeg.IsPointOnLineOnSegment(projMe);	//ͶӰ���Ƿ��������߶�����

	bool  isBallOnShootSeg = shootSeg.IsPointOnLineOnSegment(ballShootSegInterP);
	double a_m2t_2_m2b = fabs(Utils::Normalize(self2ball.dir() - finalDir)) * 180 / Param::Math::PI;
	double a_m2t_2_bsr = fabs(Utils::Normalize(antiBallVelDir - finalDir)) * 180 / Param::Math::PI;
	double a_m2t_2_bsl = fabs(Utils::Normalize(ballVelDir - finalDir)) * 180 / Param::Math::PI; 
	double a_m2b_2_bsr = fabs(Utils::Normalize(self2ball.dir() - antiBallVelDir)) * 180 / Param::Math::PI;
	double meBall_ballVelDiff=fabs(Utils::Normalize(self2ball.dir() - ballVelDir))* 180 / Param::Math::PI;

	CGeoSegment defendSeg=CGeoSegment(CGeoPoint(-Param::Field::PITCH_LENGTH/2,-Param::Field::PENALTY_AREA_WIDTH/3),CGeoPoint(-Param::Field::PITCH_LENGTH/2,Param::Field::PENALTY_AREA_WIDTH/3));
	CGeoLineLineIntersection inter=CGeoLineLineIntersection(defendSeg,ballMovingLine);
	bool canEnterDefendArea=false;
	if (ballSpeed>200)
	{
		if (inter.Intersectant()){
			CGeoPoint interPoint=inter.IntersectPoint();
			if (defendSeg.IsPointOnLineOnSegment(interPoint))
			{
				canEnterDefendArea=true;
			}
		}
	}
	canEnterDefendArea=canEnterDefendArea&&Utils::InOurPenaltyArea(self.Pos(),40);

	bool noInterInBack=false;
	if (flags & PlayerStatus::ACCURATELY){
		noInterInBack=true;
	}
	if (noInterInBack&&self.Pos().x()<-125
		&&fabs(ballVelDir)>Param::Math::PI*60/180&&fabs(ballVelDir)<Param::Math::PI*120/180)
	{
		noInterInBack=true;
	}else{
		noInterInBack=false;
	}
		
	if ( pVision->Cycle() - _lastCycle > Param::Vision::FRAME_RATE * 0.1
		//|| BallStatus::Instance()->IsBallKickedOut(runner)
		|| _lastRunner != runner){
		setState(BEGINNING);
		Touch2ChaseCnt = 0;
		Touch2InterCnt = 0;
		Chase2TouchCnt = 0;
		_backShootDir=0;
	}
	int new_state = state(), old_state = state();
	do {
		old_state = new_state;
		switch (old_state){
			case BEGINNING:
				{
					if( ballSpeed > 120 && a_m2t_2_m2b<65 && a_m2t_2_bsr<65 && self2ball.mod()>40 && a_m2b_2_bsr<20 
						&& ball2projDist/projDist>2 && projDist<75){
						if(verBos) cout<<"BEGINNING-->Touch"<<endl;
						new_state = TOUCH_KICK;
					} else if( !noInterInBack&&ballSpeed>120 &&(ballSpeed>160 && fabs(ballVelDir)/Param::Math::PI*180>105 ||meOnBallMovingSeg&&ball2projDist>80&&(ball2projDist/projDist>1.2))){
						if(verBos) cout<<"BEGINNING-->Inter"<<endl;
						new_state = INTERCEPT_KICK;
					} else{
						if(verBos) cout<<"BEGINNING-->Chase"<<endl;
						new_state = CHASE_KICK;
					}
					break;
				}
			case TOUCH_KICK:
				if( ballSpeed<100 && !RobotSensor::Instance()->IsInfraredOn(runner)
					||meBall_ballVelDiff<Param::Math::PI/2&&fabs(ballVelDir)<Param::Math::PI/2){
					if( Touch2ChaseCnt > 5){
						if(verBos) cout<<"Touch-->Chase"<<endl;
						new_state = CHASE_KICK;
						Touch2ChaseCnt = 0;
					} else{
						Touch2ChaseCnt++;
					}
				} else if ( !noInterInBack && (meBall_ballVelDiff<Param::Math::PI/3||a_m2t_2_bsr > 75 ||a_m2t_2_m2b>75)){
					if( Touch2InterCnt > 5){
						if(verBos) cout<<"Touch-->Inter"<<endl;
						new_state = INTERCEPT_KICK;
						Touch2InterCnt = 0;
					} else{
						Touch2InterCnt++;
					}
				}
				break;
			case CHASE_KICK:
				if( a_m2t_2_m2b<65 && a_m2t_2_bsr<65 && ballSpeed > 120 && self2ball.mod() > 50 && ball2projDist/projDist>1.8){
					if( Chase2TouchCnt > 1){
						if(verBos) cout<<"Chase-->Touch"<<endl;
						new_state = TOUCH_KICK;
						Chase2TouchCnt = 0;
					} else{
						Chase2TouchCnt++;
					}
				} else if(!noInterInBack && ballSpeed > 120 &&(fabs(ballVelDir)/Param::Math::PI*180>100 
					||meOnBallMovingSeg&&ball2projDist>80&&(ball2projDist/projDist>1.2))){
					if( Chase2InterCnt > 8){
						if(verBos) cout<<"Chase-->Inter"<<endl;
						new_state = INTERCEPT_KICK;
						Chase2InterCnt = 0;
					} else{
						Chase2InterCnt++;
					}
				} 
				break;
			case INTERCEPT_KICK:
				if(ballSpeed<100 && !RobotSensor::Instance()->IsInfraredOn(runner) || (a_m2t_2_m2b<45 && a_m2t_2_bsl<45)
					|| meBall_ballVelDiff<Param::Math::PI/3&&fabs(ballVelDir)<Param::Math::PI*90/180){
					if(verBos) cout<<"Inter-->Chase"<<endl;
					_out_inter = true;
					new_state = CHASE_KICK;
				} else if( canEnterDefendArea|| projDist<35 && a_m2t_2_m2b<65 && a_m2t_2_bsr<65 && self2ball.mod()>45 && a_m2b_2_bsr<15){
					if(verBos) cout<<"Inter-->INTER_TO_TOUCH"<<endl;
					new_state = INTER_TO_TOUCH;
					//double finalDirReverse = Utils::Normalize(finalDir+Param::Math::PI);	// Ŀ�귽����
					//fixPos = projMePos + Utils::Polar2Vector(Param::Vehicle::V2::PLAYER_SIZE, finalDirReverse);
				}
				break;
			case INTER_TO_TOUCH:
				if( ballSpeed<100 && !RobotSensor::Instance()->IsInfraredOn(runner)
					|| meBall_ballVelDiff<Param::Math::PI/3||a_m2t_2_bsr > 75 ||a_m2t_2_m2b>75){
					if(verBos) cout<<"INTER_TO_TOUCH-->Chase"<<endl;
					new_state = CHASE_KICK;
					//fixPos = CGeoPoint(0,0);
				}
				break;
			default:
				new_state = BEGINNING;
				break;
		}
	} while (false);
	setState(new_state);
	if(verBos) cout<<"shoot"<<new_state<<endl;
	if(_out_inter){
		_out_inter_cnt++;
		if (_out_inter_cnt < MaxOutInterDribbleCycle){
			DribbleStatus::Instance()->setDribbleCommand(runner,3);
		} else{
			_out_inter = false;
			_out_inter_cnt = 0;
			DribbleStatus::Instance()->setDribbleCommand(runner,0);
		}
	}
	

	flags = flags | PlayerStatus::DODGE_OUR_DEFENSE_BOX;
	KickDirection::Instance()->GenerateShootDir(runner,pVision->OurPlayer(runner).Pos());

	if (state()==CHASE_KICK || state()==INTERCEPT_KICK){
		finalDir = 	KickDirection::Instance()->getRawKickDir();
	}

	static bool isInBack=false;
	if (self.Pos().x()<-120){
		isInBack=true;
	}if (self.Pos().x()>-80){
		isInBack=false;
	}
	if (flags & PlayerStatus::ACCURATELY && isInBack){	
		_backShootDir=(CGeoPoint(Param::Field::PITCH_LENGTH/2,0)-self.Pos()).dir();
		finalDir=_backShootDir;
	}

	////////////////////////////////////////////////////////
	bool changeKickDir=false;
	if (rawBallPos.x()<-250){
		CGeoSegment shootSeg=CGeoSegment(rawBallPos,rawBallPos+Utils::Polar2Vector(120,finalDir));
		for (int i=1;i<=Param::Field::MAX_PLAYER;i++)
		{
			if (pVision->OurPlayer(i).Valid())
			{
				CGeoPoint proj=shootSeg.projection(pVision->OurPlayer(i).Pos());
				double dist = proj.dist(pVision->OurPlayer(i).Pos());
				if (dist<15 && shootSeg.IsPointOnLineOnSegment(pVision->OurPlayer(i).Pos())){
					changeKickDir=true;
					break;
				}
			}
		}
	}
	if (changeKickDir)
	{
		finalDir=finalDir-Utils::Sign(finalDir)*Param::Math::PI*20/180;
	}
	///////////////////////////////////////////////////

	switch (state()){
		case TOUCH_KICK:
			{
				setSubTask(PlayerRole::makeItTouchKick(runner, finalDir, false, flags));
			}
			break;
		case CHASE_KICK:
			{
				setSubTask(PlayerRole::makeItChaseKickV2(runner, finalDir, flags));
			}
			break;
		case INTERCEPT_KICK:
			{
				setSubTask(PlayerRole::makeItInterKickV3(runner,finalDir,flags));
			}
			break;
		case INTER_TO_TOUCH:
			{
				TouchKickPos::Instance()->GenerateTouchKickPos(pVision,runner,finalDir);
				CGeoPoint targetPos = TouchKickPos::Instance()->getKickPos();
				setSubTask(PlayerRole::makeItGoto(runner, targetPos, finalDir, flags));
				break;
			}
		default:
			finalDir=KickDirection::Instance()->getRawDir();
			setSubTask(PlayerRole::makeItChaseKickV2(runner, finalDir, flags));
			break;
	}

	double shootPre=task().player.kickprecision;
	if (ballSpeed<50)
	{
		shootPre=2.5*Param::Math::PI/180;
	}

	bool is_runner_dir_ok = WorldModel::Instance()->KickDirArrived(pVision->Cycle(),
		finalDir,shootPre,runner);
	if (is_runner_dir_ok || NormalPlayUtils::faceTheirGoal(pVision,runner)&&!ball.Valid()) {
		if (!task().player.ischipkick) {
			KickStatus::Instance()->setKick(runner,task().player.kickpower);
		} else {
			KickStatus::Instance()->setChipKick(runner,task().player.chipkickpower);
		}
	}
	//cout<<"isdirOk"<<NormalPlayUtils::faceTheirGoal(pVision,runner)<<endl;
	GDebugEngine::Instance()->gui_debug_line( self.Pos(),self.Pos()+Utils::Polar2Vector(1000,finalDir));

	_lastCycle = pVision->Cycle();
	_lastRunner = runner;
	CPlayerTask::plan(pVision);
}
CPlayerCommand* CShootBall::execute(const CVisionModule* pVision)
{
	if (subTask()) {
		return subTask()->execute(pVision);
	}

	return NULL;
}
