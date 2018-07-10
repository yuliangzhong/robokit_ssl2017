/*
yangda
2017.7
*/
#include "ShootoutGoalieV2.h"
#include "BallSpeedModel.h"
#include "DefPos2015.h"
#include "DefendUtils.h"
#include "DribbleStatus.h"
#include "KickStatus.h"
#include "PlayInterface.h"
#include "RobotSensor.h"
#include "TaskMediator.h"
#include "VisionModule.h"
#include "WorldModel.h"
#include "param.h"
#include "skill/Factory.h"
#include "utils.h"
#include <cmath>
#include <string>

namespace {
	/*
	BLOCK  : block shoot
	ADVANCE: intercept ball
	CLEAR  : clear ball
	RESCUE : when ball is behind goalie
	*/
	enum GOALIE_STATES {BLOCK = 1, ADVANCE = 2, CLEAR = 3, RESCUE = 4 };

	const int FIELD_WIDTH = 600;
	const int FIELD_LEN = 900;
	const int GATE_WIDTH = 100;
	const CGeoPoint LEFT_GOAL_POST =
		CGeoPoint(-Param::Field::PITCH_LENGTH / 2, -Param::Field::GOAL_WIDTH / 2);
	const CGeoPoint RIGHT_GOAL_POST =
		CGeoPoint(-Param::Field::PITCH_LENGTH / 2, Param::Field::GOAL_WIDTH / 2);
	const CGeoPoint GOAL_CENTER = CGeoPoint(-Param::Field::PITCH_LENGTH / 2, 0);
	const CGeoPoint ENEMY_GOAL_CENTER =
		CGeoPoint(Param::Field::PITCH_LENGTH / 2, 0);
	const CGeoPoint LEFT_GOAL_CENTER_FRONT =
		CGeoPoint(-Param::Field::PITCH_LENGTH / 2 + 50, -10);
	const CGeoPoint RIGHT_GOAL_CENTER_FRONT =
		CGeoPoint(-Param::Field::PITCH_LENGTH / 2 + 50, 10);
	const CGeoPoint LEFT_CORNER =
		CGeoPoint(-Param::Field::PITCH_LENGTH / 2, -Param::Field::PITCH_WIDTH / 2);
	const CGeoPoint RIGHT_CORNER =
		CGeoPoint(-Param::Field::PITCH_LENGTH / 2, Param::Field::PITCH_WIDTH / 2);
	const CGeoLine BASE_LINE = CGeoLine(LEFT_CORNER, RIGHT_CORNER);

	const bool IS_DEBUG = true;

	const int SAFE_DIST = 40;
	const int SHOOT_RADIUS = 20;
	const int INTERVAL = 60;
	const double SPEED_ANGLE = 15;

	int iRobotNum;
	PlayerVisionT me;
	MobileVisionT ball;
	PlayerVisionT him;
	double ballx;
	double bally;
	double himx;
	double himy;
	double mex;
	double mey;
	const double a = 0.3;
	const double b = 0.6;
	const double d2r = 1.0 / 180 * Param::Math::PI;

}

void CShootoutGoalieV2::setState(int state) {
	if (state == _state) return;
	_state = state;
	if (!IS_DEBUG) return;
	std::string stateStr;
	switch (_state) {
	case BLOCK: {
		stateStr = "BLOCK";
		break;
				}
	case ADVANCE: {
		stateStr = "ADVANCE";
		break;
				  }
	case CLEAR: {
		stateStr = "CLEAR";
		break;
				}
	case RESCUE: {
		stateStr = "RESCUE";
		break;
				 }
	default:
		stateStr = "ERROR: NO STATE";
		break;
	}

	std::cout << "new state: " << stateStr << endl;
	std::cout <<"current speed" << me.Vel().x() <<endl;
	std::cout <<"is about to shoot" << isAboutToShoot()<<endl;
}

CShootoutGoalieV2::CShootoutGoalieV2() { setState(ADVANCE); }

void CShootoutGoalieV2::plan(const CVisionModule *pVision) {
	// get info
	iRobotNum = task().executor;
	me = pVision->OurPlayer(iRobotNum);
	ball = pVision->Ball();
	him = pVision->TheirPlayer(BestPlayer::Instance()->getTheirBestPlayer());
	ballx = ball.Pos().x();
	bally = ball.Pos().y();
	himx = him.Pos().x();
	himy = him.Pos().y();
	mex = me.Pos().x();
	mey = me.Pos().y();

	TaskT myTask(task());
	myTask.executor = iRobotNum;

	switch (_state) {
	case BLOCK: {
		if (goalIsCovered()) {
			setState(ADVANCE);
		}
				}
	case ADVANCE: {
		if (isInDanger()) {
			setState(RESCUE);
		}
				  }
	case CLEAR: {
		if (isAboutToShoot()) {
			setState(BLOCK);
		}
		else if (isInDanger()) {
			setState(RESCUE);
		}
				}
	case RESCUE: {
		if (!isInDanger()) {
			setState(BLOCK);
		}
				 }
	default:
		setState(ADVANCE);
	}

	//execution
	switch (_state) {
	case BLOCK: {
		CGeoPoint blockPos;
		if ((ball.Pos() - LEFT_GOAL_POST).mod() < (ball.Pos() - RIGHT_GOAL_POST).mod()) {
			if (bally > LEFT_GOAL_POST.y()) {
				const CGeoLine& b2gc = CGeoLine(ball.Pos(), GOAL_CENTER);
				blockPos = b2gc.projection(me.Pos());
			}
			else {
				const CGeoLine& b2lgp = CGeoLine(ball.Pos(), LEFT_GOAL_POST);
				blockPos = b2lgp.projection(me.Pos());
			}
		}
		else {
			if (bally < RIGHT_GOAL_POST.y()) {
				const CGeoLine& b2gc = CGeoLine(ball.Pos(), GOAL_CENTER);
				blockPos = b2gc.projection(me.Pos());
			}
			else {
				const CGeoLine& b2rgp = CGeoLine(ball.Pos(), RIGHT_GOAL_POST);
				blockPos = b2rgp.projection(me.Pos());
			}
		}
		setSubTask(PlayerRole::makeItGoto(
			iRobotNum, blockPos,(ball.Pos()-me.Pos()).dir(), CVector(0,0),0,PlayerStatus::QUICKLY,0));
				}
	case ADVANCE: {
		// position
		double x, y;
		if (goalIsCovered()) {
			x = (ballx - mex) * a + mex;
			y = (bally - mey) * b + mey;
		}
		else {
			x = (ballx - GOAL_CENTER.x()) * a + GOAL_CENTER.x();
			y = (bally - GOAL_CENTER.y()) * b + GOAL_CENTER.y();
		}
		const CGeoPoint advancePos = CGeoPoint(x, y);
		// direction
		double advanceDir = (ball.Pos() - me.Pos()).dir();
		// velocity
		double velx, vely;
		if (ballx > 500) velx = 100;
		else if (ballx > 300) velx = 300;
		if (bally < 150)vely = 200;
		else if (bally < 250) vely = 100;
		const CVector& advanceVel = CVector(velx, vely);
		setSubTask(PlayerRole::makeItGoto(
			iRobotNum, advancePos, advanceDir, advanceVel, 0, PlayerStatus::QUICKLY, 0));
				  }
	case CLEAR: {
		const double clearDir = -1 * abs(him.Dir()) / him.Dir() * 60 * d2r;
		setSubTask(PlayerRole::makeItInterKickV3(
			iRobotNum, clearDir, PlayerStatus::QUICKLY | PlayerStatus::FORCE_KICK, true));
				}
	case RESCUE: {
		const CGeoPoint& rescuePos = GOAL_CENTER;
		const double rescueDir = 0;
		setSubTask(PlayerRole::makeItGoto(
			iRobotNum, rescuePos, rescueDir, CVector(0,0), 0, PlayerStatus::QUICKLY, 0));
				 }
	}
	CStatedTask::plan(pVision);
}

CPlayerCommand *CShootoutGoalieV2::execute(const CVisionModule *pVision) {
	if (subTask())
		return subTask()->execute(pVision);
	return 0;
}

bool CShootoutGoalieV2::isInDanger() {
	return ballx < mex && ballx > GOAL_CENTER.x();
}

// projection of vehicle on baseline from the ball's point of view
// approximation
bool CShootoutGoalieV2::goalIsCovered() {
	if (isInDanger())
		return false;
	double y1 = (ballx - GOAL_CENTER.x()) / (ballx - mex) *
		Param::Vehicle::V2::PLAYER_SIZE +
		bally;
	double y2 = -(ballx - GOAL_CENTER.x()) / (ballx - mex) *
		Param::Vehicle::V2::PLAYER_SIZE +
		bally;
	return y1 > RIGHT_GOAL_POST.y() && y2 < LEFT_GOAL_POST.y();
}

bool CShootoutGoalieV2::isAboutToShoot() {
	if (ballx < himx && himx < mex || mex>himx && himx>ballx) return true;
	if (bally < himy && himy < mey || mey>himy && himy>bally) return true;
	if ((ball.Pos() - him.Pos()).mod() < SHOOT_RADIUS) return true;
	if (abs(ball.Vel().dir() - him.Vel().dir()) < SPEED_ANGLE*d2r) return true;
	return false;
}
