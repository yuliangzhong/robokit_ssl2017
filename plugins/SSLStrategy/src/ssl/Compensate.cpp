#include "Compensate.h"
#include "Global.h"
#include <./tinyxml/ParamReader.h>
#include "PlayInterface.h"

#define COMPENSATION_TEST_MODE false
#define DEBUG if(COMPENSATION_TEST_MODE)

using namespace Utils;

namespace{
	bool record = false;
}

CCompensate::CCompensate(){
	readCompensateTable();
}

void CCompensate::readCompensateTable(){
	const string path = "data\\ssl\\play_books\\";
	string sCarNum;

	//cout << "reading playbooks:" << endl;
	for (int carnum = 1; carnum <= 12; carnum++)
	{
		stringstream fullname;
		fullname << path << "data-" << carnum << ".txt";
		//cout << fullname.str()<<endl;

		ifstream infile(fullname.str().c_str());
		if (!infile) {
			cerr << "error opening file data" << endl;
			exit(-1);
		}
		string line;
		int rowcount;
		int columncount;
		getline(infile, line);
		istringstream lineStream(line);
		lineStream >> rowcount >> columncount;
		for (int i = 0; i < rowcount; i++)
		{
			getline(infile, line);
			istringstream lineStream(line);
			for (int j = 0; j < columncount; j++){
				lineStream >> compensatevalue[carnum][i][j];
			}
		}
	}
}

//lua 调用接口
double CCompensate::getKickDir(int playerNum, CGeoPoint kickTarget){
	/*cout << "start" << endl;
	cout << (int)ourRobotIndex[0] <<endl;
	cout << (int)ourRobotIndex[1] << endl;
	cout << (int)ourRobotIndex[2] << endl;
	cout << (int)ourRobotIndex[3] << endl;
	cout << (int)ourRobotIndex[4] << endl;
	cout << (int)ourRobotIndex[5] << endl;
	*/
	CVisionModule* pVision = vision;
	const MobileVisionT & ball = pVision->Ball();
	const PlayerVisionT & kicker = pVision->OurPlayer(playerNum);
	double rawkickdir = (kickTarget - kicker.Pos()).dir();
	double ballspeed =ball.Vel().mod();
	double tempdir = (Normalize(Normalize(pVision->Ball().Vel().dir()+Param::Math::PI)-(kickTarget - kicker.Pos()).dir()))*180/Param::Math::PI;
	int ratio = 0;
	if (tempdir>0){
		ratio = 1;
	}else{
		ratio = -1;
	}
	double compensatevalue;
	double rawdir=
		fabs((Normalize(Normalize(pVision->Ball().Vel().dir()+Param::Math::PI)-(kickTarget - kicker.Pos()).dir()))
		*180/Param::Math::PI);


	//cout << pVision->Cycle() << endl;
	//cout << "A" << Normalize(pVision->Ball().Vel().dir() + Param::Math::PI) << endl;
	//cout << "B" << (kickTarget - kicker.Pos()).dir() << endl;
	//cout << "rawdir:"<<rawdir << endl;

	if (rawdir > 80){
		rawdir = 80;
		//cout<<"kickdirection rawdir changed to 80"<<endl;
	}
	
	int realPlayerNum = PlayInterface::Instance()->getRealIndexByNum(playerNum);
	//int realPlayerNum = ourRobotIndex[playerNum-1];

	compensatevalue = checkCompensate(realPlayerNum, ballspeed, rawdir);
	if (pVision->Ball().Vel().mod()<10){
		//cout<<"Compensate 0 because the ball's velocity is too small (<10)"<<endl;
		compensatevalue = 0;
	}

	//if(IS_SIMULATION){
	//	compensatevalue = 0;
	//}

	DEBUG cout << "----------Current Compensation---------" << endl;
	DEBUG cout << "Car Num:" << realPlayerNum << "\tTable Value:" << compensatevalue << endl;
	DEBUG cout << "Ball Speed:" << ballspeed << "\tRaw Dir:" << rawdir << endl;
	DEBUG cout << "Real Fix Value:" << ratio*compensatevalue << endl;

	double realkickdir= Utils::Normalize(Utils::Normalize(ratio*compensatevalue*Param::Math::PI/180)+rawkickdir);
	//cout<<vision->Cycle()<<" "<<ballspeed<<" "<<rawdir<<" "<<compensatevalue<<endl;
	return realkickdir;
}

//核心接口
double CCompensate::checkCompensate(int playernum,double ballspeed,double rawdir){
	double compensate = 0;
	if (ballspeed<195){
		ballspeed = 195;
	}
	if (ballspeed > 650){
		ballspeed = 650;
	}
	int column = ceil(rawdir/5);
	int  row =ceil((ballspeed-195)/5);
	if (row<1){
		row = 1;
	}
	if (column < 1){
		column = 1;
	}
	double distleft = rawdir -(column-1)*5;
	double distright = column*5 - rawdir;
	double distup = ballspeed - ((row -1)*5+195);
	double distdown = row*5+195 - ballspeed;
	double leftfactor = distright/5;
	double rightfactor = distleft/5;
	double upfactor = distdown/5;
	double downfactor = distup/5;
	compensate = (leftfactor*compensatevalue[playernum][row - 1][column - 1] + rightfactor*compensatevalue[playernum][row - 1][column])*upfactor+ (leftfactor*compensatevalue[playernum][row][column - 1] + rightfactor*compensatevalue[playernum][row][column])*downfactor;
	
	//cout << "C++ compensate:" << compensate<<endl;
	if (record == true){
		compensate = 0;
	}
	return compensate;
}

void CCompensate::setOurRobotIndex(unsigned char*_ourRobotIndex)
{
	//std::cout << "in set ourrobotindex()" << std::endl;
	for (int i = 0; i < 6; i++) {
		ourRobotIndex[i] = _ourRobotIndex[i];
		//std::cout << (int)ourRobotIndex[i] << std::endl;
	}
}