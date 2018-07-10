/*
shootout Goalie
2017.7
yangda
*/

#include "ShootoutGoalie.h"
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
	enum GOALIE_STATES { PREPARE = 1, RUSH = 2, CLEAR = 3, RESCUE = 4, FIXY = 5 };

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
	const double Y_THRESH = 20;

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
	bool YFIXED = false;
	const double d2r = 1.0 / 180 * Param::Math::PI;

	inline double max(double a, double b) {
		return a > b ? a : b;
	}
}

void CShootoutGoalie::setState(int state) {
	if (state == _state)
		return;

	_state = state;

	if (!IS_DEBUG)
		return;
	std::string stateStr;
	switch (_state) {
	case PREPARE:
		stateStr = "PREPARE";
		break;
	case RUSH:
		stateStr = "RUSH";
		break;
	case CLEAR:
		stateStr = "CLEAR";
		break;
	case RESCUE:
		stateStr = "RESCUE";
		break;
	default:
		break;
	}
	std::cout << "new state: " << stateStr << endl;
	std::cout << "is in danger: " << isInDanger() << endl;
	std::cout << "is goal covered: " << goalIsCovered() << endl;
	std::cout << "last Cycle number: " << _lastCycle << endl;
}

CShootoutGoalie::CShootoutGoalie() {
	setState(PREPARE);
	_lastCycle = 0;
	_enemyDeepestX = -150;
}

void CShootoutGoalie::plan(const CVisionModule *pVision) {
	_lastCycle++;
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

	_enemyDeepestX = _enemyDeepestX < himx ? _enemyDeepestX : himx;

	TaskT myTask(task());
	myTask.executor = iRobotNum;
	// switching
	switch (_state) {

	case PREPARE: {
		if (isAboutToShoot()) {
			if (goalIsCovered())
				setState(RUSH);
			else
				setState(RESCUE);
		}
		// else if (ball.Vel().mod() < 30 ||
		// 		 (ball.Pos() - me.Pos()).dir() - me.Vel().dir() < 10 * d2r ||
		// 		 me.Vel().mod() < 30)
		else
			setState(RUSH);
		break;
	}

	case RUSH: {
		if (isInDanger()) {
			setState(RESCUE);
		}
		else if (isAboutToShoot()) {
			if (goalIsCovered()) {
				if (canClear()) {
					setState(CLEAR);
				}
				else {
					setState(RESCUE);
				}
			}
			else {
				setState(RESCUE);
			}
		}
		else if (canClear()) {
			setState(CLEAR);
		}
		break;
	}

	case RESCUE: {
		if (!isInDanger()) {
			if (canClear()) {
				setState(CLEAR);
			}
		}
		break;
	}

	case CLEAR: {
		if (isInDanger()) {
			setState(RESCUE);
		}
		break;
	}

	}
 
	// execution
	switch (_state) {
	case PREPARE: {
		myTask.player.pos =
			DefPos2015::Instance()->getDefPos2015(pVision).getGoaliePos();
		myTask.player.angle = (him.Pos() - me.Pos()).dir();
		setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(myTask));
		break;
	}

	case RUSH: {
		CGeoPoint rush_pos = calcRushPos(pVision);
		setSubTask(PlayerRole::makeItGoto(
			iRobotNum, rush_pos, 0, CVector(100, 100), 0, 0));
		if (IS_DEBUG)
			std::cout << "current player velocity: " << me.VelX() << " y: " << me.VelY() << endl;
	}

	case RESCUE: {
		CGeoPoint rescue_pos = calcRescuePos();
		setSubTask(PlayerRole::makeItSimpleGoto(iRobotNum, GOAL_CENTER,
			0,PlayerStatus::QUICKLY));
	}

	case CLEAR: {
		KickStatus::Instance()->setKick(iRobotNum, 9999);
		// const double m2b_d = (ball.Pos() - me.Pos()).dir();
		const double clearBallDir = calcClearDir();

		// setSubTask(PlayerRole::makeItInterKickV3(iRobotNum, m2b_d, 0, 0));
		// interKickV3 does not care about the goal, this is not acceptable
		// setSubTask(PlayerRole::makeItNoneTrajGetBall(
		// 	iRobotNum, 0, CVector(0, 0), PlayerStatus::QUICKLY | PlayerStatus::FORCE_KICK));
		setSubTask(PlayerRole::makeItAdvanceBallV2(iRobotNum,PlayerStatus::QUICKLY,0));
	}

 	default:
		break;
	}

	// OUTPUT ball.vel
	if (ball.Vel().mod() > 800 && ball.VelX() < 0)
		std::cout << string(5, '\n') << "OVERSPEED shot, does not count" << endl << "current speed is: " <<
		ball.Vel().mod();

	CStatedTask::plan(pVision);
}

CPlayerCommand *CShootoutGoalie::execute(const CVisionModule *pVision) {
	if (subTask())
		return subTask()->execute(pVision);
	return 0;
}

// projection of vehicle on baseline from the ball's point of view
// approximation
bool CShootoutGoalie::goalIsCovered() {
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

// target point proposal
CGeoPoint CShootoutGoalie::calcRushPos(const CVisionModule *pVision) {
	double x, y;
	// enemy is between me and ball, hit him
	if (ballx > himx && himx > mex || bally > himy && himy > mey) {
		return CGeoPoint(himx, himy);
	}
	if (ball.VelY() > him.VelY() && ball.VelY() > 30) { 
		x = mex;
		y = abs(ball.VelX()) / ball.VelY() * Param::Field::GOAL_WIDTH / 2 + mey;
		return CGeoPoint(x, y);
	}
	x = (ballx - mex) * 0.3 + mex;
	y = (bally - mey) * 0.9 + mey;
	return CGeoPoint(x, y);
}

CGeoPoint CShootoutGoalie::calcRescuePos() {
	double x, y;
	x = min(ballx - std::abs(ballx - GOAL_CENTER.x()) * 0.1,
		-Param::Field::PITCH_LENGTH / 2);
	if (bally < LEFT_GOAL_POST.y() && ball.VelY() < 0)
		y = LEFT_GOAL_POST.y();
	else if (ball.VelY() > 0)
		y = min(RIGHT_GOAL_POST.y(), bally);
	else if (ball.VelY() < 0)
		y = max(LEFT_GOAL_POST.y(), bally);
	else
		y = RIGHT_GOAL_POST.y();
	return CGeoPoint(x, y);
}

// ball is behind the goalie
bool CShootoutGoalie::isInDanger() {
	return ballx < mex - Param::Vehicle::V2::PLAYER_SIZE;
}

// if enemy can shoot quickly
bool CShootoutGoalie::isAboutToShoot() {
	if (std::abs(him.Vel().dir() - (ball.Pos() - him.Pos()).dir()) <
		SPEED_ANGLE / 180 * Param::Math::PI)
		return ((ball.Pos() - him.Pos()).mod() < SHOOT_RADIUS);
}

// cautiously decide whether to clear or not because calling shootBall is
// dangerous
bool CShootoutGoalie::canClear() {
	// ball is slow and enemy cannot shoot right away
	double m2b_dir = (me.Pos() - ball.Pos()).dir();

	// enemy is between me and ball, don't rush
	if (ballx > himx && himx > mex) { return false; }
	if (bally > himy && himy > mey) { return false; }
	if ((ball.Pos() - me.Pos()).dir() - me.Vel().dir() < 10 * d2r &&
		!isAboutToShoot() &&
		ball.Vel().mod() < 100) {
		return true;
	}
	// ball is moving towards me and I am not too fast
	else if (abs(m2b_dir - ball.Vel().dir()) < 5 * d2r && me.VelX() < 100 &&
		ball.Vel().mod() > 30)
		return true;
	return false;
}

// velocity function
// needs improvement
CVector CShootoutGoalie::calcRushVel() {
	double x = max(300 - (ball.Pos() - me.Pos()).mod(), 0);
	double y = max(300 - (ball.Pos() - me.Pos()).mod(), 0);
	return CVector(x, y);
}
// TODO: if enemy is controlling the ball, I will not be able to see it in the
// vision info

double CShootoutGoalie::calcClearDir() {
	if (mex < ballx) return me.Dir();
	else return -90 * d2r * himy / std::abs(himy);
	//else return (him.Pos() - me.Pos()).dir();
}

