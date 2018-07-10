#ifndef _ELASTIC_BAND_H_
#define _ELASTIC_BAND_H_

#include <ANN/ANN.h>
#include <robokit/foundation/utils/geometry.h>
#include <vector>

namespace rbk{
	namespace algorithm{
		/// define bubble struct
		struct Bubble
		{
			rbk::foundation::GeoPoint	m_pos;					// circle center
			double		m_radius;				// circle radius
			rbk::foundation::GeoPoint	m_nearestObstaclePos;	// close obstacle
			double		m_routeLengthFromStart;	// accumulated length along path
			rbk::foundation::Vector		m_force;				// artificial forces
		};

		/// define interaction states
		enum InterStatusT
		{
			NO_OVERLAP = 0,
			INCLUSION,
			OVERLAP,
		};

		/// define the class
		class ElasticBand {
		public:
			/// Constructor
			ElasticBand();

			/// Destroyer
			~ElasticBand();

			void configure(double p_dist_mask = 2000,		// mm
				double p_robot_width = 450		// mm
			);
			/// Update current laser: mm
			void UpdateObstaclePoints(std::vector<rbk::foundation::GeoPoint> obstacle_points);

			/// Get the modified bubbles int robot frame reference: mm
			void GetBubblesPos(std::vector<rbk::foundation::GeoPoint> &bubblePos, std::vector<double> &bubbleRadius);

			// copy from ros eband_local_planner
			bool setPlan(const std::vector<rbk::foundation::GeoPoint>& global_plan);

			bool optimizeBand();

			void clearLaser();

			bool isPathFree(double max_dist);

			bool isPathFree(rbk::foundation::GeoPoint point1,rbk::foundation::GeoPoint point2);
			double getPathLength();
		private:
			void CalculateRouteLength();

			// copy from ros eband_local_planner
			bool convertPlanToBand(std::vector<rbk::foundation::GeoPoint> plan, std::vector<Bubble>& band);
			bool refineBand(std::vector<Bubble>& band);
			bool removeAndFill(std::vector<Bubble>& band, std::vector<Bubble>::iterator& start_iter,std::vector<Bubble>::iterator& end_iter);
			bool fillGap(std::vector<Bubble>& band, std::vector<Bubble>::iterator& start_iter,std::vector<Bubble>::iterator& end_iter);
			bool interpolateBubbles(rbk::foundation::GeoPoint start_center, rbk::foundation::GeoPoint end_center, rbk::foundation::GeoPoint& interpolated_center);
			bool checkOverlap(Bubble bubble1, Bubble bubble2);
			bool calcObstacleKinematicDistance(rbk::foundation::GeoPoint center_pose, double& distance);
			bool calcBubbleDistance(rbk::foundation::GeoPoint start_center_pose, rbk::foundation::GeoPoint end_center_pose, double& distance);
			bool optimizeBand(std::vector<Bubble>& band);
			bool modifyBandArtificialForce(std::vector<Bubble>& band);
			bool calcInternalForces(int bubble_num, const std::vector<Bubble>& band, const Bubble& curr_bubble, rbk::foundation::Vector& forces);
			bool calcExternalForces(int bubble_num, Bubble curr_bubble, rbk::foundation::Vector& forces);
			bool calcBubbleDifference(rbk::foundation::GeoPoint start_center_pose, rbk::foundation::GeoPoint end_center_pose, rbk::foundation::Vector& difference);
			bool suppressTangentialForces(int bubble_num, const std::vector<Bubble>& band, rbk::foundation::Vector& forces);
			bool applyForces(int bubble_num, std::vector<Bubble>& band, const std::vector<rbk::foundation::Vector>& forces);
			bool getForcesAt(int bubble_num, const std::vector<Bubble>& band, Bubble curr_bubble, rbk::foundation::Vector& forces);
			bool moveApproximateEquilibrium(const int& bubble_num, const std::vector<Bubble>& band, Bubble& curr_bubble,
				const rbk::foundation::Vector& curr_bubble_force, rbk::foundation::Vector& curr_step_width, const int& curr_recursion_depth);

		private:
			std::vector<Bubble> m_bubbles;

			///////////////////// ANN/////////////////////////////////////
			int					m_ann_nPts;			// actual number of data points
			ANNpointArray		m_ann_dataPts;		// data points
			ANNpoint			m_ann_queryPt;		// query point
			ANNidxArray			m_ann_nnIdx;		// near neighbor indices
			ANNdistArray		m_ann_dists;		// near neighbor distances
			ANNkd_tree*			m_ann_kdTree;	// search structure
			///////////////////////////////////////////////////////////////

			///////////////// Elastic Band Parameters /////////////////////
			double p_dist_mask_;				// critical dist for mask
			double p_robot_width_;			// width of the robot
			///////////////////////////////////////////////////////////////
			double min_bubble_overlap_;
			double tiny_bubble_expansion_; //0.01
			int num_optim_iterations_;
			double tiny_bubble_distance_ ; //0.01
			double internal_force_gain_;
			double external_force_gain_;
			double equilibrium_relative_overshoot_;
			double significant_force_; //0.15
			int max_recursion_depth_approx_equi_;
		};
	}
}

#endif	//	~_ELASTIC_BAND_H_
