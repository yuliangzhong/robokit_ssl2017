#ifndef __VISION_RECEIVER4RBK_H__
#define __VISION_RECEIVER4RBK_H__
#include "WorldDefine.h"
#include "singleton.h"
#include "playmode.h"

class COptionModule;
class CVisionReceiver4rbk{
public:
	CVisionReceiver4rbk();
	~CVisionReceiver4rbk(void);
	bool getVisionInfo(const COptionModule *,VisualInfoT&, RefRecvMsg&);
private:
	bool rawVision2VisualInfo(const COptionModule *,VisualInfoT&);
	PlayMode m_play_mode;
};
typedef NormalSingleton<CVisionReceiver4rbk> VisionReceiver4rbk;

#endif
