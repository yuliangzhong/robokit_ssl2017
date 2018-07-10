#ifndef _FILE_PARAM_H_
#define _FILE_PARAM_H_
#include <string>
namespace Param{
	namespace File{
		const std::string DataDir = "data\\ssl\\data\\";
		const char* const ParamDir = "data\\ssl\\params\\";
		const std::string RobotPosFilterDir = "data\\ssl\\vision\\Robot_Param\\Pos\\";
		const std::string RobotRotFilterDir = "data\\ssl\\vision\\Robot_Param\\Rotation\\";
		const std::string BallFilterDir = "data\\ssl\\vision\\Ball_Param\\";
	}
	namespace Output{
		const std::string NewLineCharacter = "\r\n";
	}
}
#endif // _FILE_PARAM_H_
