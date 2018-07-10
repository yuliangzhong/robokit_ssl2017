/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2016,
 *  TU Dortmund - Institute of Control Theory and Systems Engineering.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the institute nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Christoph Rösmann
 *********************************************************************/


#ifndef ROBOT_FOOTPRINT_MODEL_H
#define ROBOT_FOOTPRINT_MODEL_H

#include <robokit/algorithm/teb/teb_local_planner/pose_se2.h>
#include <robokit/algorithm/teb/teb_local_planner/obstacles.h>
//#include <visualization_msgs/Marker.h>
#include "boost/smart_ptr/make_shared.hpp"

namespace teb_local_planner
{

/**
 * @class BaseRobotFootprintModel
 * @brief Abstract class that defines the interface for robot footprint/contour models
 * 
 * The robot model class is currently used in optimization only, since
 * taking the navigation stack footprint into account might be
 * inefficient. The footprint is only used for checking feasibility.
 */
class BaseRobotFootprintModel
{
public:
  
  /**
    * @brief Default constructor of the abstract obstacle class
    */
  BaseRobotFootprintModel()
  {
  }
  
  /**
   * @brief Virtual destructor.
   */
  virtual ~BaseRobotFootprintModel()
  {
  }


  /**
    * @brief Calculate the distance between the robot and an obstacle
    * @param current_pose Current robot pose
    * @param obstacle Pointer to the obstacle
    * @return Euclidean distance to the robot
    */
  virtual double calculateDistance(const PoseSE2& current_pose, const Obstacle* obstacle) const = 0;

  /**
    * @brief Visualize the robot using a markers
    * 
    * Fill a marker message with all necessary information (type, pose, scale and color).
    * The header, namespace, id and marker lifetime will be overwritten.
    * @param current_pose Current robot pose
    * @param[out] markers container of marker messages describing the robot shape
    */
  virtual void visualizeRobot(const PoseSE2& current_pose/*, std::vector<visualization_msgs::Marker>& markers */) const {}

	

public:	
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


//! Abbrev. for shared obstacle pointers
typedef boost::shared_ptr<BaseRobotFootprintModel> RobotFootprintModelPtr;
//! Abbrev. for shared obstacle const pointers
typedef boost::shared_ptr<const BaseRobotFootprintModel> RobotFootprintModelConstPtr;



/**
 * @class PointRobotShape
 * @brief Class that defines a point-robot
 * 
 * Instead of using a CircularRobotFootprint this class might
 * be utitilzed and the robot radius can be added to the mininum distance 
 * parameter. This avoids a subtraction of zero each time a distance is calculated.
 */
class PointRobotFootprint : public BaseRobotFootprintModel
{
public:
  
  /**
    * @brief Default constructor of the abstract obstacle class
    */
  PointRobotFootprint() {}
  
  /**
   * @brief Virtual destructor.
   */
  virtual ~PointRobotFootprint() {}

  /**
    * @brief Calculate the distance between the robot and an obstacle
    * @param current_pose Current robot pose
    * @param obstacle Pointer to the obstacle
    * @return Euclidean distance to the robot
    */
  virtual double calculateDistance(const PoseSE2& current_pose, const Obstacle* obstacle) const
  {
    return obstacle->getMinimumDistance(current_pose.position());
  }

};


/**
 * @class CircularRobotFootprint
 * @brief Class that defines the a robot of circular shape
 */
class CircularRobotFootprint : public BaseRobotFootprintModel
{
public:
  
  /**
    * @brief Default constructor of the abstract obstacle class
    * @param radius radius of the robot
    */
  CircularRobotFootprint(double radius) : radius_(radius) { }
  
  /**
   * @brief Virtual destructor.
   */
  virtual ~CircularRobotFootprint() { }

  /**
    * @brief Set radius of the circular robot
    * @param radius radius of the robot
    */
  void setRadius(double radius) {radius_ = radius;}
  
  /**
    * @brief Calculate the distance between the robot and an obstacle
    * @param current_pose Current robot pose
    * @param obstacle Pointer to the obstacle
    * @return Euclidean distance to the robot
    */
  virtual double calculateDistance(const PoseSE2& current_pose, const Obstacle* obstacle) const
  {
    return obstacle->getMinimumDistance(current_pose.position()) - radius_;
  }

  /**
    * @brief Visualize the robot using a markers
    * 
    * Fill a marker message with all necessary information (type, pose, scale and color).
    * The header, namespace, id and marker lifetime will be overwritten.
    * @param current_pose Current robot pose
    * @param[out] markers container of marker messages describing the robot shape
    */
  virtual void visualizeRobot(const PoseSE2& current_pose/*, std::vector<visualization_msgs::Marker>& markers*/ ) const 
  {
    //markers.resize(1);
    //visualization_msgs::Marker& marker = markers.back();
    //marker.type = visualization_msgs::Marker::CYLINDER;
    //current_pose.toPoseMsg(marker.pose);
    //marker.scale.x = marker.scale.y = 2*radius_; // scale = diameter
    //marker.scale.z = 0.05;
    //marker.color.a = 0.5;
    //marker.color.r = 0.0;
    //marker.color.g = 0.8;
    //marker.color.b = 0.0;
  }

private:
    
  double radius_;
};


/**
 * @class TwoCirclesRobotFootprint
 * @brief Class that approximates the robot with two shifted circles
 */
class TwoCirclesRobotFootprint : public BaseRobotFootprintModel
{
public:
  
  /**
    * @brief Default constructor of the abstract obstacle class
    * @param front_offset shift the center of the front circle along the robot orientation starting from the center at the rear axis (in meters)
    * @param front_radius radius of the front circle
    * @param rear_offset shift the center of the rear circle along the opposite robot orientation starting from the center at the rear axis (in meters)
    * @param rear_radius radius of the front circle
    */
  TwoCirclesRobotFootprint(double front_offset, double front_radius, double rear_offset, double rear_radius) 
    : front_offset_(front_offset), front_radius_(front_radius), rear_offset_(rear_offset), rear_radius_(rear_radius) { }
  
  /**
   * @brief Virtual destructor.
   */
  virtual ~TwoCirclesRobotFootprint() { }

  /**
   * @brief Set parameters of the contour/footprint
   * @param front_offset shift the center of the front circle along the robot orientation starting from the center at the rear axis (in meters)
   * @param front_radius radius of the front circle
   * @param rear_offset shift the center of the rear circle along the opposite robot orientation starting from the center at the rear axis (in meters)
   * @param rear_radius radius of the front circle
   */
  void setParameters(double front_offset, double front_radius, double rear_offset, double rear_radius) 
  {front_offset_=front_offset; front_radius_=front_radius; rear_offset_=rear_offset; rear_radius_=rear_radius;}
  
  /**
    * @brief Calculate the distance between the robot and an obstacle
    * @param current_pose Current robot pose
    * @param obstacle Pointer to the obstacle
    * @return Euclidean distance to the robot
    */
  virtual double calculateDistance(const PoseSE2& current_pose, const Obstacle* obstacle) const
  {
    Eigen::Vector2d dir = current_pose.orientationUnitVec();
    double dist_front = obstacle->getMinimumDistance(current_pose.position() + front_offset_*dir) - front_radius_;
    double dist_rear = obstacle->getMinimumDistance(current_pose.position() - rear_offset_*dir) - rear_radius_;
    return std::min(dist_front, dist_rear);
  }

  /**
    * @brief Visualize the robot using a markers
    * 
    * Fill a marker message with all necessary information (type, pose, scale and color).
    * The header, namespace, id and marker lifetime will be overwritten.
    * @param current_pose Current robot pose
    * @param[out] markers container of marker messages describing the robot shape
    */
  virtual void visualizeRobot(const PoseSE2& current_pose/*, std::vector<visualization_msgs::Marker>& markers*/ ) const 
  {
//    std_msgs::ColorRGBA color;
//    color.a  = 0.5;
//    color.r  = 0.0;
//    color.g = 0.8;
//    color.b  = 0.0;
//    
//    Eigen::Vector2d dir = current_pose.orientationUnitVec();
//    if (front_radius_>0)
//    {
//      markers.push_back(visualization_msgs::Marker());
//      visualization_msgs::Marker& marker1 = markers.front();
//      marker1.type = visualization_msgs::Marker::CYLINDER;
//      current_pose.toPoseMsg(marker1.pose);
//      marker1.pose.position.x += front_offset_*dir.x();
//      marker1.pose.position.y += front_offset_*dir.y();
//      marker1.scale.x = marker1.scale.y = 2*front_radius_; // scale = diameter
////       marker1.scale.z = 0.05;
//      marker1.color = color;
//
//    }
//    if (rear_radius_>0)
//    {
//      markers.push_back(visualization_msgs::Marker());
//      visualization_msgs::Marker& marker2 = markers.back();
//      marker2.type = visualization_msgs::Marker::CYLINDER;
//      current_pose.toPoseMsg(marker2.pose);
//      marker2.pose.position.x -= rear_offset_*dir.x();
//      marker2.pose.position.y -= rear_offset_*dir.y();
//      marker2.scale.x = marker2.scale.y = 2*rear_radius_; // scale = diameter
////       marker2.scale.z = 0.05;
//      marker2.color = color;
//    }
  }

private:
    
  double front_offset_;
  double front_radius_;
  double rear_offset_;
  double rear_radius_;
  
};



/**
 * @class LineRobotFootprint
 * @brief Class that approximates the robot with line segment (zero-width)
 */
class LineRobotFootprint : public BaseRobotFootprintModel
{
public:
  
  /**
    * @brief Default constructor of the abstract obstacle class
    * @param line_start start coordinates (only x and y) of the line (w.r.t. robot center at (0,0))
    * @param line_end end coordinates (only x and y) of the line (w.r.t. robot center at (0,0))
    */
  LineRobotFootprint( rbk::foundation::Point& line_start,  rbk::foundation::Point& line_end)
  {
    setLine(line_start, line_end);
  }
  
  /**
  * @brief Default constructor of the abstract obstacle class (Eigen Version)
  * @param line_start start coordinates (only x and y) of the line (w.r.t. robot center at (0,0))
  * @param line_end end coordinates (only x and y) of the line (w.r.t. robot center at (0,0))
  */
  LineRobotFootprint(const Eigen::Vector2d& line_start, const Eigen::Vector2d& line_end)
  {
    setLine(line_start, line_end);
  }
  
  /**
   * @brief Virtual destructor.
   */
  virtual ~LineRobotFootprint() { }

  /**
   * @brief Set vertices of the contour/footprint
   * @param vertices footprint vertices (only x and y) around the robot center (0,0) (do not repeat the first and last vertex at the end)
   */
  void setLine( rbk::foundation::Point& line_start,  rbk::foundation::Point& line_end)
  {
    line_start_.x() = line_start.x(); 
    line_start_.y() = line_start.y(); 
    line_end_.x() = line_end.x();
    line_end_.y() = line_end.y();
  }
  
  /**
   * @brief Set vertices of the contour/footprint (Eigen version)
   * @param vertices footprint vertices (only x and y) around the robot center (0,0) (do not repeat the first and last vertex at the end)
   */
  void setLine(const Eigen::Vector2d& line_start, const Eigen::Vector2d& line_end)
  {
    line_start_ = line_start; 
    line_end_ = line_end;
  }
  
  /**
    * @brief Calculate the distance between the robot and an obstacle
    * @param current_pose Current robot pose
    * @param obstacle Pointer to the obstacle
    * @return Euclidean distance to the robot
    */
  virtual double calculateDistance(const PoseSE2& current_pose, const Obstacle* obstacle) const
  {
    // here we are doing the transformation into the world frame manually
    double cos_th = std::cos(current_pose.theta());
    double sin_th = std::sin(current_pose.theta());
    Eigen::Vector2d line_start_world;
    line_start_world.x() = current_pose.x() + cos_th * line_start_.x() - sin_th * line_start_.y();
    line_start_world.y() = current_pose.y() + sin_th * line_start_.x() + cos_th * line_start_.y();
    Eigen::Vector2d line_end_world;
    line_end_world.x() = current_pose.x() + cos_th * line_end_.x() - sin_th * line_end_.y();
    line_end_world.y() = current_pose.y() + sin_th * line_end_.x() + cos_th * line_end_.y();
    return obstacle->getMinimumDistance(line_start_world, line_end_world);
  }

  /**
    * @brief Visualize the robot using a markers
    * 
    * Fill a marker message with all necessary information (type, pose, scale and color).
    * The header, namespace, id and marker lifetime will be overwritten.
    * @param current_pose Current robot pose
    * @param[out] markers container of marker messages describing the robot shape
    */
  virtual void visualizeRobot(const PoseSE2& current_pose/*, std::vector<visualization_msgs::Marker>& markers */) const 
  {   
    //std_msgs::ColorRGBA color;
    //color.a  = 0.5;
    //color.r  = 0.0;
    //color.g = 0.8;
    //color.b  = 0.0;
  
    //markers.push_back(visualization_msgs::Marker());
    //visualization_msgs::Marker& marker = markers.front();
    //marker.type = visualization_msgs::Marker::LINE_STRIP;
    //current_pose.toPoseMsg(marker.pose); // all points are transformed into the robot frame!
    //
    //// line
    //rbk::protocol::Point line_start_world;
    //line_start_world.x = line_start_.x();
    //line_start_world.y = line_start_.y();
    //line_start_world.z = 0;
    //marker.points.push_back(line_start_world);
    //
    //rbk::protocol::Point line_end_world;
    //line_end_world.x = line_end_.x();
    //line_end_world.y = line_end_.y();
    //line_end_world.z = 0;
    //marker.points.push_back(line_end_world);

    //marker.scale.x = 0.05; 
    //marker.color = color;
  }

private:
    
  Eigen::Vector2d line_start_;
  Eigen::Vector2d line_end_;
  
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
};



/**
 * @class PolygonRobotFootprint
 * @brief Class that approximates the robot with a closed polygon
 */
class PolygonRobotFootprint : public BaseRobotFootprintModel
{
public:
  
  
  
  /**
    * @brief Default constructor of the abstract obstacle class
    * @param vertices footprint vertices (only x and y) around the robot center (0,0) (do not repeat the first and last vertex at the end)
    */
  PolygonRobotFootprint(const Point2dContainer& vertices) : vertices_(vertices) { }
  
  /**
   * @brief Virtual destructor.
   */
  virtual ~PolygonRobotFootprint() { }

  /**
   * @brief Set vertices of the contour/footprint
   * @param vertices footprint vertices (only x and y) around the robot center (0,0) (do not repeat the first and last vertex at the end)
   */
  void setVertices(const Point2dContainer& vertices) {vertices_ = vertices;}
  
  /**
    * @brief Calculate the distance between the robot and an obstacle
    * @param current_pose Current robot pose
    * @param obstacle Pointer to the obstacle
    * @return Euclidean distance to the robot
    */
  virtual double calculateDistance(const PoseSE2& current_pose, const Obstacle* obstacle) const
  {
    // here we are doing the transformation into the world frame manually
    double cos_th = std::cos(current_pose.theta());
    double sin_th = std::sin(current_pose.theta());
    Point2dContainer polygon_world(vertices_.size());
    for (std::size_t i=0; i<vertices_.size(); ++i)
    {
      polygon_world[i].x() = current_pose.x() + cos_th * vertices_[i].x() - sin_th * vertices_[i].y();
      polygon_world[i].y() = current_pose.y() + sin_th * vertices_[i].x() + cos_th * vertices_[i].y();
    }
    return obstacle->getMinimumDistance(polygon_world);
  }

  /**
    * @brief Visualize the robot using a markers
    * 
    * Fill a marker message with all necessary information (type, pose, scale and color).
    * The header, namespace, id and marker lifetime will be overwritten.
    * @param current_pose Current robot pose
    * @param[out] markers container of marker messages describing the robot shape
    */
  virtual void visualizeRobot(const PoseSE2& current_pose/*, std::vector<visualization_msgs::Marker>& markers */) const 
  {
    //if (vertices_.empty())
    //  return;
    //
    //std_msgs::ColorRGBA color;
    //color.a  = 0.5;
    //color.r  = 0.0;
    //color.g = 0.8;
    //color.b  = 0.0;
  
    //markers.push_back(visualization_msgs::Marker());
    //visualization_msgs::Marker& marker = markers.front();
    //marker.type = visualization_msgs::Marker::LINE_STRIP;
    //current_pose.toPoseMsg(marker.pose); // all points are transformed into the robot frame!
    //
    //for (std::size_t i = 0; i < vertices_.size(); ++i)
    //{
    //  rbk::protocol::Point point;
    //  point.x = vertices_[i].x();
    //  point.y = vertices_[i].y();
    //  point.z = 0;
    //  marker.points.push_back(point);
    //}
    //// add first point again in order to close the polygon
    //rbk::protocol::Point point;
    //point.x = vertices_.front().x();
    //point.y = vertices_.front().y();
    //point.z = 0;
    //marker.points.push_back(point);

    //marker.scale.x = 0.025; 
    //marker.color = color;

  }

private:
    
  Point2dContainer vertices_;
  
};





} // namespace teb_local_planner

#endif /* ROBOT_FOOTPRINT_MODEL_H */
