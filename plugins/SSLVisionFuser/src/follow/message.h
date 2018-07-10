#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define SendCarNum  6
#define ReceiveCarNum  12
#define MaxFollowNum 12
#define MaxBallNum  6
#define MaxNumOfCamera 4

#define BLUE 0
#define YELLOW 1
#define ERRORNUM 9999

#define PI 3.1415926
#define INITNUM int(-1)

const int MaxValidLength=4750;
const int MaxValidWidth=3250;

#define  CALIERROR 100
#define  BALLCALIERROR 200
enum COURT{LeftUp,RightUp,LeftDown,RightDown};
enum CAMERAMODE{SingleCamLeftUp,SingleCamRightUp,SingleCamLeftDown,SingleCamRightDown,TwoCamUp,TwoCamDown,FourCamera};

typedef struct SENDVISIONMESSAGE
{
	int Cycle;
    bool  BallFound;
	float Ballx;
	float Bally;

    int nCameraID;
	int BallImagex;
	int BallImagey;

	unsigned char  RobotINDEX[2][SendCarNum];
	bool  RobotFound[2][SendCarNum];
	float RobotPosX[2][SendCarNum];
	float RobotPosY[2][SendCarNum];
	float RobotRotation[2][SendCarNum];

}SendVisionMessage;


typedef struct RECEIVEVISIONMESSAGE
{
	int Cycle;
	//added by ouyangbo
	//double capture_time;
	bool  BallFound[MaxBallNum];
	float Ballx[MaxBallNum];
	float Bally[MaxBallNum];
	float BallConfidence[MaxBallNum];//С???????Ŷ?
	//int nCameraID;//0????????1??????
	//int BallImagex;
	//int BallImagey;

	unsigned char  RobotINDEX[2][ReceiveCarNum];
	bool  RobotFound[2][ReceiveCarNum];
	float RobotPosX[2][ReceiveCarNum];
	float RobotPosY[2][ReceiveCarNum];
	float RobotRotation[2][ReceiveCarNum];
	float RobotConfidence[2][ReceiveCarNum];

	RECEIVEVISIONMESSAGE(){
		for(int i = 0; i<MaxBallNum; i++){
			BallFound[i] = false;
			Ballx[i] = -ERRORNUM;
			Bally[i] = -ERRORNUM;
		}
		for(int j = 0;j < ReceiveCarNum; j++){
			for(int i=0;i<2;i++){
				RobotINDEX[i][j] = ReceiveCarNum + 1;
				RobotFound[i][j] = false;
				RobotPosX[i][j] = RobotPosY[i][j] = -ERRORNUM;
				RobotRotation[i][j] = 0;
				RobotConfidence[i][j] = 0;
			}
		}
	}
}ReceiveVisionMessage;

typedef struct  FOLLOWARRAY
{
	int Num;
	int LostFrame;
	int AddFrame;
	bool Fresh;
	float PosX;
	float PosY;
	float Rotation;
	float Confidence;
	bool Send;
}FollowArray;
#endif
