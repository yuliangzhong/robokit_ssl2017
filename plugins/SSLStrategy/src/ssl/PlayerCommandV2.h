#ifndef _PLAYER_COMMAND_V2_H_
#define _PLAYER_COMMAND_V2_H_
#include <PlayerCommand.h>
#include "src_cmd.pb.h"
/************************************************************************/
/*                         Speed                                        */
/************************************************************************/
class CPlayerSpeedV2 : public CPlayerCommand{
public:
	CPlayerSpeedV2(int number, double xspeed, double yspeed, double rspeed, unsigned char dribble);
	virtual CVector getAffectedVel() const;
	virtual double getAffectedRotateSpeed() const;
	virtual void toStream(std::ostream& os) const;
	inline void setSpeedtoSSLCmd(rbk::protocol::Message_SSL_Command* cmd){
		if (cmd == nullptr) {
			std::cout << "Error in setSpeedtoSSLCmd , cmd is null!!!" <<std::endl;
			return;
		}
		cmd->set_velocity_x(xspeed()/100.0);
		cmd->set_velocity_y(yspeed()/100.0);
		cmd->set_velocity_r(rspeed()/1.000);
		cmd->set_dribbler_spin(dribble()/3.0);
	}
protected:
	double xspeed() const{ return _xspeed; }
	double yspeed() const{ return _yspeed; }
	double rspeed() const{ return _rspeed; }
	int commandType() const;
private:
	double _xspeed;
	double _yspeed;
	double _rspeed;
};

/************************************************************************/
/*                         Kick                                         */
/************************************************************************/
class CPlayerKickV2 : public CPlayerCommand{
public:
	CPlayerKickV2(int number, double normal, double chip, double pass, unsigned char dribble)
		: CPlayerCommand(number, dribble), _normal(normal), _chip(chip), _pass(pass){ }
	virtual void toStream(std::ostream& os) const;
	inline void setKicktoSSLCmd(rbk::protocol::Message_SSL_Command* cmd){
		if (cmd == nullptr) {
			std::cout << "Error in setSpeedtoSSLCmd , cmd is null!!!" <<std::endl;
			return;
		}
		cmd->set_flat_kick(_normal/100.0);
		cmd->set_chip_kick(_chip/100.0);
	}
private:
	double _normal; // 普通击球的力度
	double _chip; // 挑球的距离
	double _pass; // 传球的距离
};
/************************************************************************/
/*                         Gyro(陀螺仪)                                 */
/************************************************************************/
class CPlayerGyroV2 : public CPlayerCommand{
public:
	CPlayerGyroV2(int number, unsigned char dribble, unsigned int radius, double angle,  double rspeed)
		: CPlayerCommand(number, dribble), _radius(radius), _angle(angle), _rspeed(rspeed){ }
	virtual void toStream(std::ostream& os) const;
protected:
	double radius() const { return _radius; }
	double angle() const { return _angle; }
	double rspeed() const { return _rspeed; }
private:
	unsigned int _radius;	// 旋转半径
	double _angle;			// 旋转角度
	double _rspeed;			// 旋转速度
};
#endif
