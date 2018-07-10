#ifndef _QUADTREE_PATHPLANNER_H_
#define _QUADTREE_PATHPLANNER_H_

#include <robokit/algorithm/quadtree_pathplan/elastic_band.h>
#include <robokit/foundation/utils/pose_se2.h>
#include <robokit/foundation/map_reader/map_misc.h>
#include <robokit/foundation/quadtree/quadtree.h>
#include <set>
#include <vector>
#include <string>
#include <iostream>

namespace rbk{
	namespace algorithm{
		// AStar Search Node
		enum PathPlanResT {
			PP_RES_NONE = 0,			///< default
			PP_RES_INITIALIZED,			///< initialize
			PP_RES_SUCCEED,				///< succeed plan
			PP_RES_END_CANNOT_REACH,	///< end point invalid
			PP_RES_BEGIN_CANNOT_OUT,	///< start point invalid
			PP_RES_FAILED,				///< failed plan
		};

		struct Cell_Node {
			stg_cell_t *cell;
			Cell_Node * parent;
			double fn;
			double gn;
			double hn;

			bool operator < (const Cell_Node& rhs) const {
				return this->fn < rhs.fn;
			}
		};

		/// Quad-tree based planning
		class CQuadTreePathPlanner{
		public:
			/// Constructor
			CQuadTreePathPlanner();

			/// Destructor
			~CQuadTreePathPlanner() {}

			void Initialize(int resulotion, double x_max, double x_min, double y_max, double y_min,std::vector<rbk::foundation::GeoPoint> points,std::vector<rbk::foundation::GeoLine> lines);

			rbk::foundation::QuadTree* getQuadTree();

			void SetResolutionAndRange(int res_, rbk::foundation::Range range_);

			/// Update current laser
			void UpdateObstaclePoints(std::vector<rbk::foundation::GeoPoint> obstacle_points);

			void setLaserOffset(double o);

			/// Do path plan with two-boundary states
			// both states are in world coordinate
			// (x,y,theta) -> (mm,mm,rad)
			void DoPathPlanning(
				const rbk::foundation::PoseSE2& initial_state,
				const rbk::foundation::PoseSE2& terminal_state,
				bool force_replan = false);

			/// Is Succeed
			bool PlanSucceed();

			void setRobotWidthCritialSizeAndExtraSafeDist(double width, double safe_dist = 10);

			/// Check last path
			bool CheckPath(const rbk::foundation::PoseSE2& initial_state,const rbk::foundation::PoseSE2& terminal_state);

			/// Is free in map?
			bool IsPathFreeInMap(rbk::foundation::GeoPoint p1, rbk::foundation::GeoPoint p2);

			/// Interface to modify the map
			void ModifyCurrentMap();

			/// Interface to real algorithm
			void ExecuteAlgorithm();

			/// Interface to optimize the path
			std::vector<rbk::foundation::PoseSE2> OptimizePath(bool, std::vector<rbk::foundation::PoseSE2>);

			/// Find the min path
			void FindMinPathInMap(std::vector<rbk::foundation::PoseSE2>&, std::vector<rbk::foundation::PoseSE2>::iterator&, std::vector<rbk::foundation::PoseSE2>::iterator&);

			/// Reset the path
			void ResetPath(const rbk::foundation::PoseSE2& initial_state,const rbk::foundation::PoseSE2& terminal_state);

			/// Get the current available path
			// all waypoints are in world coordinate
			// List[(x,y,theta)] -> List[(mm,mm,rad)]
			// [initial, ...->... ,terminal]
			std::vector< rbk::foundation::PoseSE2 >& GetPath();

			std::vector< rbk::foundation::PoseSE2 >& GetFreePath();

			std::vector<rbk::foundation::PoseSE2>& GetRawPath();

			std::vector< double >& GetRadiusList();

			void setLaserSeg(std::vector<rbk::foundation::GeoSegment>);

			double getPathLength();

			//void setDev(NPlugin* dev){ m_dev = dev;}
			/// Interface to optimize the path
			//virtual vector<Message_NavState> OptimizePath(bool use_laser, std::vector< Message_NavState > path);
			bool is_repath;
			//NPlugin* m_dev;
		private:
			/// Child list
			std::vector< stg_cell_t* > m_childlist;

			/// Open list
			std::set< Cell_Node > m_openlist;

			/// Close list
			std::set< Cell_Node > m_closelist;

			std::string								m_planner_name;
			PathPlanResT						m_plan_result;
			std::vector<rbk::foundation::GeoPoint> 			m_obstacle_points;
			rbk::foundation::PoseSE2								m_initial_state;
			rbk::foundation::PoseSE2								m_terminal_state;
			std::vector< rbk::foundation::PoseSE2 >			m_available_path;
			std::vector< rbk::foundation::PoseSE2 >			m_free_available_path;
			std::vector< rbk::foundation::PoseSE2 >			m_raw_available_path;
			std::vector< double >			m_waypoints_radius;
			double									m_laser_offset;
			std::vector< rbk::foundation::GeoSegment>	m_laser_seg;
			double									m_robot_width;
			double									m_extra_safe_dist; // total safe dist = m_robot_width/2.0 + m_extra_safe_dist
			double									m_safe_dist;
			ElasticBand							elastic_band;
			rbk::foundation::QuadTree*							m_quad_tree;
			std::vector<rbk::foundation::GeoLine>			m_map_line_list;
		};
	}
}
#endif	// ~_QUADTREE_PATHPLANNER_H_
