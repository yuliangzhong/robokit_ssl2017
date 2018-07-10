#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "VisionModule.h"
#include "DribbleStatus.h"
#include "KickStatus.h"
#include "GDebugEngine.h"
#include "WorldModel.h"
#include "singleton.h"
#include "KickDirection.h"
#include "DefPos2015.h"
#include "DefPos2013.h"
#include "TandemPos.h"
#include "ChipBallJudge.h"
#include "gpuBestAlgThread.h"
#include "BestPlayer.h"
#include "defence/DefenceInfo.h"
#include "IndirectDefender.h"
#include "SupportPos.h"

extern CVisionModule*  vision;
extern CKickStatus*    kickStatus;
extern CDribbleStatus* dribbleStatus;
extern CGDebugEngine*  debugEngine;
extern CWorldModel*	   world;
extern CKickDirection* kickDirection;
extern CGPUBestAlgThread* bestAlg;
extern CDefPos2015* defPos2015;
extern CDefPos2013* defPos2013;
extern CTandemPos* tandemPos;
extern CBestPlayer* bestPlayer;
extern CDefenceInfo* defenceInfo;
extern CChipBallJudge* chipBallJudge;
extern CIndirectDefender* indirectDefender;
extern CSupportPos* supportPos;
void initializeSingleton();
#endif