#ifndef __SHOOTOUT_GOALIE__V2__
#define __SHOOTOUT_GOALIE__V2__
#include "skill/PlayerTask.h"

class CShootoutGoalieV2 : public CStatedTask {
 public:
	CShootoutGoalieV2();
	virtual void plan(const CVisionModule* pVision);
	virtual bool isEmpty() const { return false; };
	void setState(int state);
	virtual CPlayerCommand* execute(const CVisionModule* pVision);

 protected:
	virtual void toStream(std::ostream &os) const {
		os << "Skill: CShootoutGoalie" << std::endl;
	}
	bool isInDanger();
	bool isAboutToShoot();
	bool goalIsCovered();
 private:
	int _state;
};

#endif // __SHOOTOUT_GOALIE__
