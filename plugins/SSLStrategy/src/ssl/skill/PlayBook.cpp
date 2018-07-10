#include "PlayBook.h"
#include "BasicPlay.h"
#include <OptionModule.h>
#include <VisionModule.h>
#include <PlayInterface.h>
#include <TaskMediator.h>

#include "TestSkillPlay.h"

//FILE OUT
#include <fstream>


//#define USE_FOREVER_FREEKICK  // ʹ��������������ս��
//#define USE_SPECIAL_PENALTY // ʹ������ӵ���
//#define USE_IMBA_DEFEND_FREEKICK // ʹ����������ŷ���
//#define __NO__DEBUG__
#ifdef __NO__DEBUG__
static ofstream nullfile("NUL");
static ostream& output = nullfile;
#else
static ostream& output = cerr; 
#endif

namespace{
	int DEMO_MODE = 0;
	bool VERBOSE_MODE = false;

	 // ���һ��play��applicatable=false���ֳ������ʱ�䣬������ѡ��play
	const int TRANSMIT_TIME = 0.2 * Param::Vision::FRAME_RATE;
	template < class PlayType > CBasicPlay* makePlay() {
		static PlayType _instance;
		return &_instance;
	}
}

CPlayBook::CPlayBook(const COptionModule* pOption, const CVisionModule* pVision)
: _pOption(pOption), _pVision(pVision),_currentPlay(0)
{
	DECLARE_PARAM_READER_BEGIN(CPlayBook)
		READ_PARAM(DEMO_MODE)
	DECLARE_PARAM_READER_END
}

static bool byScore(CBasicPlay* play1, CBasicPlay* play2) 
{
	return play1->score() < play2->score();
}

CBasicPlay* CPlayBook::getPlay()
{
	///>2. ----����ģʽ/����ģʽ----
	if( DEMO_MODE != 0 ){	
		switch( DEMO_MODE ){

#define MAKE_PLAY(num, name) \
	case num: \
		_currentPlay = makePlay<name> (); \
		break;
		MAKE_PLAY(1,CTestSkillPlay);
		
		default:
			break;
		}
		return _currentPlay;
	}

	return NULL;
}

bool CPlayBook::handleSpecialPlays()
{
	return false;
}



const char* CPlayBook::currentPlayName() const
{
	if( _currentPlay ){
		return _currentPlay->playName();
	}
	return "No Play";
}