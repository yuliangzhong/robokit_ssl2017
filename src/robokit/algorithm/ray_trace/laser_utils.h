#ifndef _LASER_UTILS_H_
#define _LASER_UTILS_H_

#include <vector>
#include <robokit/foundation/quadtree/quadtree_basic.h>
#include <robokit/foundation/map_reader/map_misc.h>

namespace rbk
{
	namespace algorithm
	{

		namespace LaserUtils{
			struct VirtualLaserBeam{
				double angle;
				double dist;
				bool is_vitual;
				bool is_valid;
			};
			struct VirtualLaser{
				std::vector<VirtualLaserBeam> virtual_laser_beams;
			};
			VirtualLaser SimulateVirtualLaser(
				stg_matrix_t* matrix, int resolution, rbk::foundation::Range map_range, 
				const rbk::foundation::GeoPoint& laser_pos,const double& initial_state_angle ,
				const double angle_range, const double angle_step, double max_laser_range);
			bool IsObstacleLaserPoint(stg_matrix_t* matrix,  int resolution, rbk::foundation::Range range, double x, double y);
		}
	}
}

#endif
