#include "Global.h"

CVisionModule*   vision;
CKickStatus*    kickStatus;
CDribbleStatus* dribbleStatus;
CGDebugEngine*  debugEngine;
CWorldModel*	world;
CKickDirection* kickDirection;
CGPUBestAlgThread* bestAlg;
CDefPos2015* defPos2015;
CDefPos2013* defPos2013;
CIndirectDefender* indirectDefender;
CTandemPos* tandemPos;
CBestPlayer* bestPlayer;
CDefenceInfo* defenceInfo;
CChipBallJudge* chipBallJudge;
CSupportPos* supportPos;


void initializeSingleton()
{
	vision			= VisionModule::Instance();
	kickStatus		= KickStatus::Instance();
	dribbleStatus	= DribbleStatus::Instance();
	debugEngine		= GDebugEngine::Instance();
	world			= WorldModel::Instance();
	kickDirection	= KickDirection::Instance();
	bestAlg			= GPUBestAlgThread::Instance();
	supportPos		= SupportPos::Instance();
	defPos2015		= DefPos2015::Instance();
	defPos2013		= DefPos2013::Instance();
	bestPlayer		= BestPlayer::Instance();
	defenceInfo		= DefenceInfo::Instance();
	tandemPos		= TandemPos::Instance();
	chipBallJudge	= ChipBallJudge::Instance();
	indirectDefender = IndirectDefender::Instance();
}