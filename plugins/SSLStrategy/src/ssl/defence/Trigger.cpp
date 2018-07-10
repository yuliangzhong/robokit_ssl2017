#include "Trigger.h"
#include "DefenceInfo.h"
#include <math.h>
#include <algorithm>
#include "WorldModel.h"
bool CTrigger::handler(const CVisionModule* pVision)
{
	if (true == this->trigger(pVision))
	{
		return true;
	} else if (NULL != _next)
	{
		return _next->handler(pVision);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//make trigger

namespace{};

DESCRIPT_TRIGGER(testTrigger1)
{
	//cout<<" trigger test 1  is false"<<endl;
	return false;
}

DESCRIPT_TRIGGER(testTrigger2)
{
	//cout<<" trigger test 2  is true"<<endl;
	return false;
}

DESCRIPT_TRIGGER(TKeeperChanged)
{
	bool changed = false;
	static int lastBallKeeper = 0;
	//compare
	if (lastBallKeeper != DefenceInfo::Instance()->getAttackOppNumByPri(0))
	{
		//cout<<"keeper changed is "<<changed<<endl;
		changed = true;
	}
	//update
	lastBallKeeper = DefenceInfo::Instance()->getAttackOppNumByPri(0);
	//return
	
	return changed;
}

DESCRIPT_TRIGGER(TAttackerChanged)
{
	static int lastAttackNum = 0;
	static int lastAttackArray[Param::Field::MAX_PLAYER] = {0,0,0,0,0,0};
	bool changed = false;
	const int attackNum = DefenceInfo::Instance()->getAttackNum();
	//compare
	if (lastAttackNum != attackNum)
	{
		changed = true;
		//cout<<"ssssssssssssss"<<endl;
	} 
	else{
		sort(lastAttackArray,lastAttackArray + attackNum);
		int tempArray[Param::Field::MAX_PLAYER] = {0,0,0,0,0,0};
		for (int i = 0;i <= attackNum - 1;++i)
			tempArray[i] = DefenceInfo::Instance()->getAttackOppNumByPri(i);
		sort(tempArray,tempArray + attackNum);
		int i = 0;
		for (;i < attackNum;++i)
		{
			if (lastAttackArray[i] != tempArray[i])
			{
				changed = true;
				//cout<<"true!!!!!!!!!!!!!!!!!!!"<<endl;
				break;
			}
		}
	} 
	//update
	lastAttackNum = DefenceInfo::Instance()->getAttackNum();
	for (int i = 0;i < Param::Field::MAX_PLAYER;++i)
	{
		lastAttackArray[i] = DefenceInfo::Instance()->getAttackOppNumByPri(i);
	}
	//return

	return changed;
}

DESCRIPT_TRIGGER(TReceiverNoMarked)
{
	const string refMsg = WorldModel::Instance()->CurrentRefereeMsg();
	if ("theirIndirectKick" == refMsg || "theirDirectKick" == refMsg || "theirKickOff" == refMsg || VisionModule::Instance()->gameState().gameOn() )
	{
		if (DefenceInfo::Instance()->getAttackNum() >= 2){
			for (int i=1;i<=2;i++)
			{
				if (DefenceInfo::Instance()->getOppPlayerByNum(i)->getRoleName()=="RReceiver" && DefenceInfo::Instance()->getAttackNum() >= 2){
					//cout<<"RReceiver "<<i<<endl;
					if(false == DefenceInfo::Instance()->queryMarked(DefenceInfo::Instance()->getAttackOppNumByPri(i))){
						return true;
					}
				}
			}
		}
		//if (DefenceInfo::Instance()->getAttackNum() >= 2 && false == DefenceInfo::Instance()->queryMarked(DefenceInfo::Instance()->getAttackOppNumByPri(1)))
		//{
		//	//cout<<"receiverNoMarked is 1"<<endl;
		//	return true;
		//}
	}
	return false;
}

DESCRIPT_TRIGGER(TReceiverNoMarked2)
{
	const string refMsg = WorldModel::Instance()->CurrentRefereeMsg();
	if ("theirIndirectKick" == refMsg || "theirDirectKick" == refMsg || "theirKickOff" == refMsg)
	{
		if (DefenceInfo::Instance()->getAttackNum() >= 3 && false == DefenceInfo::Instance()->queryMarked(DefenceInfo::Instance()->getAttackOppNumByPri(2)))
		{
			//cout<<"receiverNoMarked2 is 1"<<endl;
			return true;
		}
	}
	return false;
}