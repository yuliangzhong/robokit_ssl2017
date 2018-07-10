#include "PlayerCommandV2.h"
#include <utils.h>
#include <sstream>
#include <robot_power.h>
#include <RobotCapability.h>
#include "PlayInterface.h"
#include "TaskMediator.h"

namespace{
	const bool SPEED_MODE = true;
	inline int merge_command_type(int type, unsigned char dribble)
	{
		if( dribble > DRIBBLE_DISABLED){
			type |= 0x08;
		}
		return type;
	}
	const double RSPEED_DRIBBLEHIGH = 1;	// ����ת�ٶȴ��ڴ�ֵʹ��

	const double ACTION_ERROR = 1.0; // ִ�����
}

/************************************************************************/
/*                      CPlayerSpeed                                    */
/************************************************************************/
CPlayerSpeedV2::CPlayerSpeedV2(int number, double xspeed, double yspeed, double rspeed, unsigned char dribble)
	: CPlayerCommand(number,dribble)
	,_rspeed(rspeed)
{
	const CRobotCapability* robotCap = RobotCapFactory::Instance()->getRobotCap(1, 1);
	CVector velocity(xspeed, yspeed);

	// �����ָ��������С�����˶�����֮�� added by qxz 08.06.07
	if (number == PlayInterface::Instance()->getNumbByRealIndex(TaskMediator::Instance()->goalie())) {
		if (velocity.mod() > robotCap->maxSpeedGoalie(0))
			velocity = Utils::Polar2Vector(robotCap->maxSpeedGoalie(0), velocity.dir());
	} else if (TaskMediator::Instance()->leftBack() != 0 && number == TaskMediator::Instance()->leftBack()
		|| TaskMediator::Instance()->rightBack() != 0 && number == TaskMediator::Instance()->rightBack()
		|| TaskMediator::Instance()->singleBack() != 0 && number == TaskMediator::Instance()->singleBack()
		|| TaskMediator::Instance()->sideBack() != 0 && number == TaskMediator::Instance()->sideBack()) {
			if (velocity.mod() > robotCap->maxSpeedBack(0))
				velocity = Utils::Polar2Vector(robotCap->maxSpeedBack(0), velocity.dir());
	} else {
		if (velocity.mod() > robotCap->maxSpeed(0))
			velocity = Utils::Polar2Vector(robotCap->maxSpeed(0), velocity.dir());
	}

	_xspeed = velocity.x();
	_yspeed = velocity.y();
}

void CPlayerSpeedV2::toStream(std::ostream& os) const
{
	os << xspeed() << "\t" << yspeed() << "\t" << rspeed();
}
CVector CPlayerSpeedV2::getAffectedVel() const
{
	return CVector(xspeed(), yspeed());
}
double CPlayerSpeedV2::getAffectedRotateSpeed() const
{
	return rspeed();
}

/************************************************************************/
/*                          Kick                                        */
/************************************************************************/

void CPlayerKickV2::toStream(std::ostream& os) const
{
	os << number();
	if( _normal ){
		if( _pass ){
			os << " pass " << _pass;
		}else{
			os << " kick " << _normal;
		}
	}else{
		os << " chip kick " << _chip;
	}
}

/************************************************************************/
/*                         Gyro(������)                                 */
/************************************************************************/

void CPlayerGyroV2::toStream(std::ostream& os) const
{
	os << "rotNum "<< number()<<"radius "<<_radius<<"angle "<<_angle;
}