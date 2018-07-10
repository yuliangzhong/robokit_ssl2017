#ifndef _DYNAMIC_WINDOW_APPROACH_H_
#define _DYNAMIC_WINDOW_APPROACH_H_

#include <robokit/foundation/utils/geo_utils.h>
#include <robokit/foundation/utils/geometry.h>
#include <robokit/foundation/utils/pose_se2.h>
#include <vector>

namespace rbk
{
	namespace algorithm
	{
		class DWAMotionControl {
		public:
			DWAMotionControl();	
			~DWAMotionControl();
			void init(double head, double tail, double width,double max_speed, double max_acc, double max_rot, double max_rot_acc,double turn_back_angle, double go_angle);
			
			void SetMoveLimit(double max_speed, double max_acc, double max_rot, double max_rot_acc);
			
			void SetGoAndTrunAngle(double turn_back_angle, double go_angle);
			
			void SetReachDistAndAngle(double, double);
			void UpdateObstaclePoints(std::vector<rbk::foundation::GeoPoint> & obs_points);
			
			void GenerateCurrentVelocity(rbk::foundation::PoseSE2 initial_state,
				rbk::foundation::PoseSE2 terminal_state,
				double cur_x, double cur_y, double cur_w,
				double &vx, double &vy, double &vw, bool is_real_final);
			bool isReached();
		private:
			bool DoActionNormal(double cur_x, double cur_y, double cur_w, double &v_best , double &w_best , bool is_real_final);
			bool DoActionReached(double &v_best , double &w_best, double cur_w);
			//
			double zeroFinalVelocityTheta(double, double, double);
			void bangBangPathProperties(double z0, double zf, double vz0, double ubar, double &t1, double &t2, double &tf, double &u);
			void PredictPose(rbk::foundation::PoseSE2 &endPos , double v , double w , double t);
			double CalculateHeadingScore( const rbk::foundation::PoseSE2 &predictPos );
			double CalculateClearanceScore(double v , double w , double t);
			double CalculateSpeedScore(double v , double w);
			double CalculateSpeedScore2(double v , double w, bool is_real_final);
			double CalculateScore(double v , double w , double predict_collision_time, bool is_real_final);
			double PredictCollision(double v , double w , const rbk::foundation::GeoPoint &obstacle , const rbk::foundation::GeoPoint &a0 , const rbk::foundation::GeoPoint &a1);
			double PredictCollision(double v , double w);
			void compute_motion_1d(double x0, double v0, double v1,
				double a_max, double d_max, double v_max, double a_factor,
				double &traj_accel, double &traj_time);
			double compute_stop(double v, double max_a);
		private:
			double			m_cur_dir;
			rbk::foundation::PoseSE2	m_target;
			rbk::foundation::PoseSE2	m_last_terminal;
			rbk::foundation::Vector			m_vecRobot2Target;
			double p_step_time ;		//决策周期
			double p_max_speed;		//轮子最大线速度
			double p_max_acc;		//轮子最大线加速度
			double p_max_rot;		//最大角速度
			double p_max_rot_acc;	//最大角加速度

			double p_tMax;			//碰撞时间最大值，大于此值可忽略
			double p_heading_predict_coef_;
			double p_coef_heading;
			double p_coef_clearance;
			double p_coef_speed;
			int max_target_opposite_cnt;
			double m_turn_angle;
			double m_go_angle;
			bool m_reach_flag;
			int m_not_reach_counter;
			int m_target_opposite_cnt;
			std::vector< rbk::foundation::GeoPoint >		m_obstacle_points;
			std::vector< rbk::foundation::GeoPoint >		newGeometry;
			bool m_is_reach_fire;
			double m_reach_dist;
			double m_reach_angle;
		};
	}
}

#endif	// ~_DYNAMIC_WINDOW_APPROACH_H_
