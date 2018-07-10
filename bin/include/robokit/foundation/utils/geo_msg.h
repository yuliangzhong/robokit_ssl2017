#ifndef _GEO_MSG_H_
#define _GEO_MSG_H_

#include <iostream>
#include <iomanip>
#include <list>
#include <cmath>
#include <string>
#include <vector>
#include <assert.h>
#include <robokit/foundation/utils/quaternion.h>
#include <robokit/foundation/utils/vector3.h>
#include <robokit/foundation/utils/scalar.h>
#include <robokit/foundation/utils/matrix_3x3.h>
#include <robokit/foundation/utils/min_max.h>
#include <robokit/foundation/utils/transform.h>
#include <robokit/foundation/utils/quad_word.h>

namespace rbk
{
    namespace foundation
    {
        class Header {
        public:
            Header() :_seq(0), _sec(0), _nsec(0), _frame_id("") {}
            Header(double seq, double sec, double nsec, std::string frame_id) :_seq(seq), _sec(sec), _nsec(nsec), _frame_id(frame_id) {}
            Header(const Header& h) :_seq(h.seq()), _sec(h.sec()), _nsec(h.nsec()), _frame_id(h.frame_id()) {};
            double seq() const { return _seq; }
            double sec() const { return _sec; }
            double nsec() const { return _nsec; }
            std::string frame_id() const { return _frame_id; }
            void set_seq(double seq) { _seq = seq; }
            void set_sec(double sec) { _sec = sec; }
            void set_nsec(double nsec) { _nsec = nsec; }
            void set_frame_id(std::string frame_id) { _frame_id = frame_id; }
        private:
            double _seq;
            double _sec;
            double _nsec;
            std::string _frame_id;
        };

        class Point {
        public:
            Point() :_x(0), _y(0), _z(0) {}
            Point(double x, double y, double z) :_x(x), _y(y), _z(z) {}
            Point(const Point& p) :_x(p.x()), _y(p.y()), _z(p.z()) {}
            double x()  const { return _x; }
            double y()  const { return _y; }
            double z()  const { return _z; }
            void set_x(double x) { _x = x; }
            void set_y(double y) { _y = y; }
            void set_z(double z) { _z = z; }
        private:
            double _x;
            double _y;
            double _z;
        };

        class PointStamped {
        public:
            PointStamped() {}

            PointStamped(const Header& header, const Point& point) :_header(header), _point(point) {}

            PointStamped(const PointStamped& p) :_header(p.header()), _point(p.point()) {}

            const Header& header() const { return _header; }

            const Point& point() const { return _point; }

            Header& mutable_header() { return _header; }

            Point& mutable_point() { return _point; }
        private:
            Header _header;
            Point _point;
        };

        class Polygon {
        public:
            Polygon() {}
            Polygon(const std::vector<Point>& points) :_points(points) {}
            Polygon(const Polygon& p) :_points(p.points()) {}

            const std::vector<Point>& points() const { return _points; }
            std::vector<Point>& mutable_points() { return _points; }
        private:
            std::vector<Point> _points;
        };

        class PolygonStamped {
        public:
            PolygonStamped() {}
            PolygonStamped(const Header& header, const Polygon& polygon) :_header(header), _polygon(polygon) {}
            PolygonStamped(const PolygonStamped& p) :_header(p.header()), _polygon(p.polygon()) {}
            Header& mutable_header() { return _header; }
            Polygon& mutable_polygon() { return _polygon; }
            const Header& header() const { return _header; }
            const Polygon& polygon() const { return _polygon; }
        private:
            Header _header;
            Polygon _polygon;
        };

        class Quaternion {
        public:
            Quaternion() :_x(0), _y(0), _z(0), _w(1) {}
            Quaternion(double x, double y, double z, double w) :_x(x), _y(y), _z(z), _w(w) {}
            Quaternion(const Quaternion& q) :_x(q.x()), _y(q.y()), _z(q.z()), _w(q.w()) {}
            double x()  const { return _x; }
            double y()  const { return _y; }
            double z()  const { return _z; }
            double w() const { return _w; }
            void set_x(double x) { _x = x; }
            void set_y(double y) { _y = y; }
            void set_z(double z) { _z = z; }
            void set_w(double w) { _w = w; }
        private:
            double _x;
            double _y;
            double _z;
            double _w;
        };

        class QuaternionStamped {
        public:
            QuaternionStamped() {}
            QuaternionStamped(const Header& header, const Quaternion& quaternion) :_header(header), _quaternion(quaternion) {}
            QuaternionStamped(const QuaternionStamped& q) :_header(q.header()), _quaternion(q.quaternion()) {}

            Header& mutable_header() { return _header; }
            Quaternion& mutable_quaternion() { return _quaternion; }
            const Header& header()const { return _header; }
            const Quaternion& quaternion()const { return _quaternion; }
        private:
            Header _header;
            Quaternion _quaternion;
        };

        class Pose {
        public:
            Pose() {}
            Pose(const Point& position, const Quaternion& orientation) :_position(position), _orientation(orientation) {}
            Pose(const Pose& p) :_position(p.position()), _orientation(p.orientation()) {}
            Point&  mutable_position() { return _position; }
            Quaternion&  mutable_orientation() { return _orientation; }
            const Point&  position() const { return _position; }
            const Quaternion&  orientation() const { return _orientation; }
        private:
            Point _position;
            Quaternion _orientation;
        };

        class PoseArray {
        public:
            PoseArray() {}
            PoseArray(const Header& header, const std::vector<Pose>& poses) :_header(header), _poses(poses) {}
            PoseArray(const PoseArray& p) :_header(p.header()), _poses(p.poses()) {}
            Header& mutable_header() { return _header; }
            std::vector<Pose>& mutable_poses() { return _poses; }
            const Header& header() const { return _header; }
            const std::vector<Pose>& poses() const { return _poses; }

        private:
            Header _header;
            std::vector<Pose> _poses;
        };

        class PoseStamped {
        public:
            PoseStamped() {}
            PoseStamped(const Header& header, const Pose& pose) :_header(header), _pose(pose) {}
            PoseStamped(const PoseStamped& p) :_header(p.header()), _pose(p.pose()) {}

            Pose& mutable_pose() { return _pose; }
            Header& mutable_header() { return _header; }
            const Pose& pose()  const { return _pose; }
            const Header& header() const { return _header; }
        private:
            Header _header;
            Pose _pose;
        };

        class PoseWithCovariance {
        public:
            PoseWithCovariance() {}
            PoseWithCovariance(const Pose& pose, const std::vector<double>& covariance) :_pose(pose), _covariance(covariance) {}
            PoseWithCovariance(const PoseWithCovariance& p) :_pose(p.pose()), _covariance(p.covariance()) {}
            Pose& mutable_pose() { return _pose; }
            std::vector<double>& mutable_covariance() { return _covariance; }
            const Pose& pose() const { return _pose; }
            const std::vector<double>& covariance()const { return _covariance; }
        private:
            Pose _pose;
            std::vector<double> _covariance;
        };

        class PoseWithCovarianceStamped {
        public:
            PoseWithCovarianceStamped() {}
            PoseWithCovarianceStamped(const Header& header, const PoseWithCovariance& pose) :_header(header), _pose(pose) {}
            PoseWithCovarianceStamped(const PoseWithCovarianceStamped& p) :_header(p.header()), _pose(p.pose()) {}
            Header& mutable_header() { return _header; }
            PoseWithCovariance& mutable_pose() { return _pose; }
            const Header& header()const { return _header; }
            const PoseWithCovariance& pose()const { return _pose; }
        private:
            Header _header;
            PoseWithCovariance _pose;
        };

        class Vector3 {
        public:
            Vector3() :_x(0), _y(0), _z(0) {}
            Vector3(double x, double y, double z) :_x(x), _y(y), _z(z) {}
            Vector3(const Vector3& v) :_x(v.x()), _y(v.y()), _z(v.z()) {}
            double x() const { return _x; }
            double y() const { return _y; }
            double z() const { return _z; }
            void set_x(double x) { _x = x; }
            void set_y(double y) { _y = y; }
            void set_z(double z) { _z = z; }
        private:
            double _x;
            double _y;
            double _z;
        };

        class Vector3Stamped {
        public:
            Vector3Stamped() {}
            Vector3Stamped(const Header& header, const Vector3& v) :_header(header), _vector(v) {}
            Vector3Stamped(const Vector3Stamped& v) :_header(v.header()), _vector(v.vector()) {}
            Header& mutable_header() { return _header; }
            Vector3& mutable_vector() { return _vector; }
            const Header& header()const { return _header; }
            const Vector3& vector()const { return _vector; }
        private:
            Header _header;
            Vector3 _vector;
        };

        class Transform {
        public:
            Transform() {}
            Transform(const Vector3& v, const Quaternion& q) :_translation(v), _rotation(q) {}
            Transform(const Transform& t) :_translation(t.translation()), _rotation(t.rotation()) {}
            Vector3& mutable_translation() { return _translation; }
            Quaternion& mutable_rotation() { return _rotation; }
            const Vector3& translation()const { return _translation; }
            const Quaternion& rotation()const { return _rotation; }
        private:
            Vector3 _translation;
            Quaternion _rotation;
        };

        class TransformStamped {
        public:
            TransformStamped() {}
            TransformStamped(const Header& header, const std::string& frame_id, const Transform& transform) :_header(header), _child_frame_id(frame_id), _transform(transform) {}

            TransformStamped(const TransformStamped& t) :_header(t.header()), _child_frame_id(t.child_frame_id()), _transform(t.transform()) {}

            Header& mutable_header() { return _header; }

            std::string& mutable_child_frame_id() { return _child_frame_id; }

            Transform& mutable_transform() { return _transform; }

            const Header& header()const { return _header; }

            const  std::string& child_frame_id()const { return _child_frame_id; }

            const Transform& transform()const { return _transform; }
        private:
            Header _header;
            std::string _child_frame_id;
            Transform _transform;
        };

        class Twist {
        public:
            Twist() {};
            Twist(const Vector3& l, const Vector3& a) :_linear(l), _angular(a) {}
            Twist(const Twist& t) :_linear(t.linear()), _angular(t.angular()) {}
            Vector3&  mutable_linear() { return _linear; }
            Vector3& mutable_angular() { return _angular; }
            const Vector3&  linear()  const { return _linear; }
            const Vector3& angular()const { return _angular; }
        private:
            Vector3 _linear;
            Vector3 _angular;
        };

        class TwistStamped {
        public:
            TwistStamped() {}
            TwistStamped(const Header& h, const Twist& t) :_header(h), _twist(t) {}
            TwistStamped(const TwistStamped& t) :_header(t.header()), _twist(t.twist()) {}
            Header& header() { return _header; }
            Twist& twist() { return _twist; }
            const Header& header()const { return _header; }
            const Twist& twist()const { return _twist; }
        private:
            Header _header;
            Twist _twist;
        };

        class TwistWithCovariance {
        public:
            TwistWithCovariance() {}
            TwistWithCovariance(const Twist& t, const std::vector<double>& c) :_twist(t), _covariance(c) {}
            TwistWithCovariance(const TwistWithCovariance& t) :_twist(t.twist()), _covariance(t.covariance()) {}
            Twist& mutable_twist() { return _twist; }
            std::vector<double>& mutable_covariance() { return _covariance; }
            const Twist& twist() const { return _twist; }
            const std::vector<double>& covariance() const { return _covariance; }

        private:
            Twist _twist;
            std::vector<double> _covariance;
        };

        class TwistWithCovarianceStamped {
        public:
            TwistWithCovarianceStamped() {}
            TwistWithCovarianceStamped(const Header& h, const TwistWithCovariance& t) :_header(h), _twist(t) {}
            TwistWithCovarianceStamped(const TwistWithCovarianceStamped& t) :_header(t.header()), _twist(t.twist()) {}

            Header& mutable_header() { return _header; }
            TwistWithCovariance& mutable_twist() { return _twist; }

            const Header& header()const { return _header; }
            const TwistWithCovariance& twist()const { return _twist; }
        private:
            Header _header;
            TwistWithCovariance _twist;
        };
    }
}
#endif // ~_GEO_MSG_H_
