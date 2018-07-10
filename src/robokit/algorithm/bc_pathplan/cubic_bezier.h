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

		class NCubicBezier {
		public:
			NCubicBezier();
			// PosStart_ .. ConPoint0_ .. ConPoint1_ .. PosEnd_
			// 构造时注意4个点的顺序
			NCubicBezier(rbk::foundation::GeoPoint sp, rbk::foundation::GeoPoint cp0, rbk::foundation::GeoPoint cp1, rbk::foundation::GeoPoint ep);
			
			bool IsValid();

			void SetStartPoint(const rbk::foundation::GeoPoint &);
			void SetEndPoint(const rbk::foundation::GeoPoint &);
			void SetControlPoint0(const rbk::foundation::GeoPoint &);
			void SetControlPoint1(const rbk::foundation::GeoPoint &);
			rbk::foundation::GeoPoint GetStartPoint();
			rbk::foundation::GeoPoint GetEndPoint();

			// 在曲线上找到离给定点最近的点
			rbk::foundation::GeoPoint getPointOnLineClosestTo(const rbk::foundation::GeoPoint &pos);
			// 不建议使用！！
			// 在曲线上找到离给定点最近的点的内部ID号
			int getIdPointOnLineClosestTo(const rbk::foundation::GeoPoint &pos);
			int getIdPointOnLineClosestTo2(const rbk::foundation::GeoPoint &pos);
			// 返回以给定点开始，增加一定距离后的曲线上的点，如果超出端点，则返回端点, dist可正可负
			// 修改这个函数有可能影响到已有功能，可能导致搜路搜不到20130904
			rbk::foundation::GeoPoint getPointOnLineDistanceTo(const rbk::foundation::GeoPoint &pos, double dist);
			// exten控制返回点是否可以超出线本身，在线的切线方向延长线上
			rbk::foundation::GeoPoint getPointOnLineDistanceTo2(const rbk::foundation::GeoPoint &pos, double dist, bool exten = false);
			// 返回曲线上到给定点最近点的距离
			double getDistanceWithPoint(const rbk::foundation::GeoPoint &pos);
			// 返回给定点在曲线上投影点到起始点PosStart_的长度
			double getDistance2StartPoint(const rbk::foundation::GeoPoint &pos);
			// 返回曲线长度
			double getLength();
			// 返回在百分点处的切向方向, 规整到-180°~180°
			double getDirection(double t);
			// 返回插值点
			std::vector<rbk::foundation::GeoPoint> getPath() {
				return path_;
			}
			// 返回距离起始点t的点坐标,t(0.0~1.0)
			rbk::foundation::GeoPoint pointAtPercent(double t);
			// 返回给定点在曲线上的投影点P，P在曲线上的长度比例(0.0~1.0)
			double percentOfPoint(rbk::foundation::GeoPoint p);

		public:
			// 根据4个点计算轨迹
			static const int frac_ = 1000;
			void cal(int num);
			//	void cal_curvature();
			// PosStart_ .. ConPoint0_ .. ConPoint1_ .. PosEnd_
			rbk::foundation::GeoPoint PosStart_;
			rbk::foundation::GeoPoint PosEnd_;
			rbk::foundation::GeoPoint ConPoint0_;
			rbk::foundation::GeoPoint ConPoint1_;
			std::vector<rbk::foundation::GeoPoint> path_;

			std::vector<float>	curvature_;
			double max_c_;		// 最大曲率(deg), 曲率半径R = 1 / Deg2Rad(max_c_) in (mm)
			double min_c_;		// 最小曲率
			double mean_c_;		// 平均曲率
		};
	}
}

#endif // _CUBIC_BEZIER_H_ 
