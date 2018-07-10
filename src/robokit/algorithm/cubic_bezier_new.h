#ifndef _CUBIC_BEZIER_H_
#define _CUBIC_BEZIER_H_

#include <math.h>
#include <vector>
#include <robokit/foundation/utils/geometry.h>

namespace rbk{
	namespace algorithm{
		struct EdgeParams {
			rbk::foundation::GeoPoint start_point_;
			rbk::foundation::GeoPoint control_point0_;
			rbk::foundation::GeoPoint control_point1_;
			rbk::foundation::GeoPoint end_point_;
		};

		class CubicBezier {
		public:
			CubicBezier();
			// PosStart_ .. ConPoint0_ .. ConPoint1_ .. PosEnd_
			// ����ʱע��4�����˳��
			CubicBezier(rbk::foundation::GeoPoint sp, rbk::foundation::GeoPoint cp0, rbk::foundation::GeoPoint cp1, rbk::foundation::GeoPoint ep);
			
			bool IsValid();

			void SetStartPoint(const rbk::foundation::GeoPoint &);
			void SetEndPoint(const rbk::foundation::GeoPoint &);
			void SetControlPoint0(const rbk::foundation::GeoPoint &);
			void SetControlPoint1(const rbk::foundation::GeoPoint &);
			rbk::foundation::GeoPoint GetStartPoint();
			rbk::foundation::GeoPoint GetEndPoint();

			// ���������ҵ������������ĵ�
			rbk::foundation::GeoPoint getPointOnLineClosestTo(const rbk::foundation::GeoPoint &pos);
			// ������ʹ�ã���
			// ���������ҵ������������ĵ���ڲ�ID��
			int getIdPointOnLineClosestTo(const rbk::foundation::GeoPoint &pos);
			int getIdPointOnLineClosestTo2(const rbk::foundation::GeoPoint &pos);
			// �����Ը����㿪ʼ������һ�������������ϵĵ㣬��������˵㣬�򷵻ض˵�, dist�����ɸ�
			// �޸���������п���Ӱ�쵽���й��ܣ����ܵ�����·�Ѳ���20130904
			rbk::foundation::GeoPoint getPointOnLineDistanceTo(const rbk::foundation::GeoPoint &pos, double dist);
			// exten���Ʒ��ص��Ƿ���Գ����߱�������ߵ����߷����ӳ�����
			rbk::foundation::GeoPoint getPointOnLineDistanceTo2(const rbk::foundation::GeoPoint &pos, double dist, bool exten = false);
			// ѡ��������ʱ�������ʵĴ�С����dist���ҵ�����С��turnangle�ĽǶ�
			rbk::foundation::GeoPoint getPointInAngleOnLineDistanceTo2(const rbk::foundation::GeoPoint &pos, double curangle, double dist, double turnangle);
			// ���������ϵ������������ľ���
			double getDistanceWithPoint(const rbk::foundation::GeoPoint &pos);
			// ���ظ�������������ͶӰ�㵽��ʼ��PosStart_�ĳ���
			double getDistance2StartPoint(const rbk::foundation::GeoPoint &pos);
			// �������߳���
			double getLength();
			// �����ڰٷֵ㴦��������, ������-180��~180��
			// ����������������
			double getDirection(double t);
			// 
			double getDirection(rbk::foundation::GeoPoint);
			// ���ز�ֵ��
			std::vector<rbk::foundation::GeoPoint> getPath() {
				return path_;
			}
			std::vector<float> getCurvature() {
				return curvature_;
			}
			double getmax_c() {
				return max_c_;
			}
			int getmax_c_ind() {
				return max_c_ind_;
			}
			// ���ؾ�����ʼ��t�ĵ�����,t(0.0~1.0)
			rbk::foundation::GeoPoint pointAtPercent(double t);
			// ���ظ������������ϵ�ͶӰ��P��P�������ϵĳ��ȱ���(0.0~1.0)
			double percentOfPoint(rbk::foundation::GeoPoint p);

		public:
			// ����4�������켣
			static const int frac_ = 1000;
			void cal(int num = 1000);
			void cal_curvature();
			// PosStart_ .. ConPoint0_ .. ConPoint1_ .. PosEnd_
			rbk::foundation::GeoPoint PosStart_;
			rbk::foundation::GeoPoint PosEnd_;
			rbk::foundation::GeoPoint ConPoint0_;
			rbk::foundation::GeoPoint ConPoint1_;
			
			bool m_backward;
			int m_movestyle;
			double m_max_speed;
			double m_max_acc;
			double m_max_rot;
			double m_max_rot_acc;
			double m_reach_dist;
			double m_reach_angle;
			double m_slowdown_dist;
			double m_block_dist;
			int m_start_id;
			int m_end_id;
			
			std::vector<rbk::foundation::GeoPoint> path_;

			std::vector<float>	curvature_;
			double max_c_;		// �������(deg), ���ʰ뾶R = 1 / Deg2Rad(max_c_) in (mm)
			double min_c_;		// ��С����
			int max_c_ind_;
			int min_c_ind_;
			double mean_c_;		// ƽ������
		};
	}
}

#endif // _CUBIC_BEZIER_H_ 
