#ifndef __SHOOTOUT_GOALIE__
#define __SHOOTOUT_GOALIE__
#include "skill/PlayerTask.h"

class CShootoutGoalie : public CStatedTask {
public:
	CShootoutGoalie();
	virtual void plan(const CVisionModule* pVision);
	virtual bool isEmpty() const { return false; };
	void setState(int state);
	virtual CPlayerCommand* execute(const CVisionModule* pVision);

protected:
	virtual void toStream(std::ostream &os) const {
		os << "Skill: CShootoutGoalie" << std::endl;
	}
	bool canClear();
	bool isInDanger();
	bool isAboutToShoot();
	bool goalIsCovered();
	CVector calcRushVel();
	double calcClearDir();
	CGeoPoint calcRushPos(const CVisionModule* pVision);
	CGeoPoint calcRescuePos();
private:
	int _state;
	bool _hasBallBeenOut;
	int _lastCycle;
	double _enemyDeepestX;
};

#endif // __SHOOTOUT_GOALIE__
