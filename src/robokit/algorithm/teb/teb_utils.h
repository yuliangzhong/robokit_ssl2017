#ifndef _TEB_UTILS_H_
#define _TEB_UTILS_H_

#include <robokit/foundation/utils/geo_msg.h>

namespace teb_utils{
	double getYaw(const rbk::foundation::Quaternion& msg_q);
	rbk::tf::Quaternion createQuaternionFromYaw(double yaw);
}

#endif