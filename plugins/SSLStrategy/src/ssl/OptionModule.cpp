#include "OptionModule.h"
#include <param.h>
#include "WorldModel.h"
#include <robokit/core/rbk_config.h>

COptionModule::COptionModule()
{
	bool isRightSide = true;
	bool isYellowTeam = false;
	rbk::Config::Instance()->get("rightSide", isRightSide);
	rbk::Config::Instance()->get("yellowTeam", isYellowTeam);
	if (!isRightSide) {
		_side = Param::Field::POS_SIDE_LEFT;
	} else{
		_side = Param::Field::POS_SIDE_RIGHT;
	}

	if (isYellowTeam) {
		_color = TEAM_YELLOW;
	} else{
		_color = TEAM_BLUE;
	}
	std::cout << "Side : " << ((_side == Param::Field::POS_SIDE_LEFT) ? "left" : "right")
		<< ", Color : " << ((_color == TEAM_YELLOW) ? "yellow" : "blue") << " is running..." << Param::Output::NewLineCharacter;
	WorldModel::Instance()->registerOption(this);
}

COptionModule::~COptionModule(void)
{

}