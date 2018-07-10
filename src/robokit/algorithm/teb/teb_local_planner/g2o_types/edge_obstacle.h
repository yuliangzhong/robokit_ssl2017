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
 * Notes:
 * The following class is derived from a class defined by the
 * g2o-framework. g2o is licensed under the terms of the BSD License.
 * Refer to the base class source for detailed licensing information.
 *
 * Author: Christoph Rösmann
 *********************************************************************/
#ifndef EDGE_OBSTACLE_H_
#define EDGE_OBSTACLE_H_

#include <robokit/algorithm/teb/teb_local_planner/obstacles.h>
#include <robokit/algorithm/teb/teb_local_planner/robot_footprint_model.h>
#include <robokit/algorithm/teb/teb_local_planner/g2o_types/vertex_pose.h>
#include <robokit/algorithm/teb/teb_local_planner/g2o_types/penalties.h>
#include <robokit/algorithm/teb/teb_local_planner/teb_config.h>

#include "g2o/core/base_unary_edge.h"


namespace teb_local_planner
{

/**
 * @class EdgeObstacle
 * @brief Edge defining the cost function for keeping a minimum distance from obstacles.
 * 
 * The edge depends on a single vertex \f$ \mathbf{s}_i \f$ and minimizes: \n
 * \f$ \min \textrm{penaltyBelow}( dist2point ) \cdot weight \f$. \n
 * \e dist2point denotes the minimum distance to the point obstacle. \n
 * \e weight can be set using setInformation(). \n
 * \e penaltyBelow denotes the penalty function, see penaltyBoundFromBelow() \n
 * @see TebOptimalPlanner::AddEdgesObstacles
 * @remarks Do not forget to call setTebConfig() and setObstacle()
 */     
class EdgeObstacle : public g2o::BaseUnaryEdge<1, const Obstacle*, VertexPose>
{
public:
    
  /**
   * @brief Construct edge.
   */    
  EdgeObstacle() 
  {
    _measurement = NULL;
    _vertices[0] =NULL;
  }
 
  /**
   * @brief Destruct edge.
   * 
   * We need to erase vertices manually, since we want to keep them even if TebOptimalPlanner::clearGraph() is called.
   * This is necessary since the vertices are managed by the Timed_Elastic_Band class.
   */   
  virtual ~EdgeObstacle() 
  {
    if(_vertices[0]) 
      _vertices[0]->edges().erase(this);
  }

  /**
   * @brief Actual cost function
   */    
  void computeError()
  {
//    ROS_ASSERT_MSG(cfg_ && _measurement && robot_model_, "You must call setTebConfig(), setObstacle() and setRobotModel() on EdgeObstacle()");
    const VertexPose* bandpt = static_cast<const VertexPose*>(_vertices[0]);

    double dist = robot_model_->calculateDistance(bandpt->pose(), _measurement);

    _error[0] = penaltyBoundFromBelow(dist, cfg_->obstacles.min_obstacle_dist, cfg_->optim.penalty_epsilon);

//    ROS_ASSERT_MSG(std::isfinite(_error[0]), "EdgeObstacle::computeError() _error[0]=%f\n",_error[0]);
  }

#ifdef USE_ANALYTIC_JACOBI
#if 0

  /**
   * @brief Jacobi matrix of the cost function specified in computeError().
   */
  void linearizeOplus()
  {
    ROS_ASSERT_MSG(cfg_, "You must call setTebConfig on EdgePointObstacle()");
    const VertexPose* bandpt = static_cast<const VertexPose*>(_vertices[0]);
    
    Eigen::Vector2d deltaS = *_measurement - bandpt->position(); 
    double angdiff = atan2(deltaS[1],deltaS[0])-bandpt->theta();
    
    double dist_squared = deltaS.squaredNorm();
    double dist = sqrt(dist_squared);
    
    double aux0 = sin(angdiff);
    double dev_left_border = penaltyBoundFromBelowDerivative(dist*fabs(aux0),cfg_->obstacles.min_obstacle_dist,cfg_->optim.penalty_epsilon);

    if (dev_left_border==0)
    {
      _jacobianOplusXi( 0 , 0 ) = 0;
      _jacobianOplusXi( 0 , 1 ) = 0;
      _jacobianOplusXi( 0 , 2 ) = 0;
      return;
    }
    
    double aux1 = -fabs(aux0) / dist;
    double dev_norm_x = deltaS[0]*aux1;
    double dev_norm_y = deltaS[1]*aux1;
    
    double aux2 = cos(angdiff) * g2o::sign(aux0);
    double aux3 = aux2 / dist_squared;
    double dev_proj_x = aux3 * deltaS[1] * dist;
    double dev_proj_y = -aux3 * deltaS[0] * dist;
    double dev_proj_angle = -aux2;
    
    _jacobianOplusXi( 0 , 0 ) = dev_left_border * ( dev_norm_x + dev_proj_x );
    _jacobianOplusXi( 0 , 1 ) = dev_left_border * ( dev_norm_y + dev_proj_y );
    _jacobianOplusXi( 0 , 2 ) = dev_left_border * dev_proj_angle;
  }
#endif
#endif
  
  /**
   * @brief Compute and return error / cost value.
   * 
   * This method is called by TebOptimalPlanner::computeCurrentCost to obtain the current cost.
   * @return 1D Cost / error vector
   */   
  ErrorVector& getError()
  {
    computeError();
    return _error;
  }
  
  /**
   * @brief Read values from input stream
   */    
  virtual bool read(std::istream& is)
  {
  // is >> _measurement[0] >> _measurement[1];
    return true;
  }

  /**
   * @brief Write values to an output stream
   */ 
  virtual bool write(std::ostream& os) const
  {
  // os << information()(0,0) << " Error: " << _error[0] << ", Measurement X: " << _measurement[0] << ", Measurement Y: " << _measurement[1];
    return os.good();
  }
  
  /**
   * @brief Set pointer to associated obstacle for the underlying cost function 
   * @param obstacle 2D position vector containing the position of the obstacle
   */ 
  void setObstacle(const Obstacle* obstacle)
  {
    _measurement = obstacle;
  }
    
  /**
   * @brief Set pointer to the robot model 
   * @param robot_model Robot model required for distance calculation
   */ 
  void setRobotModel(const BaseRobotFootprintModel* robot_model)
  {
    robot_model_ = robot_model;
  }
    
  /**
   * @brief Assign the TebConfig class for parameters.
   * @param cfg TebConfig class
   */   
  void setTebConfig(const TebConfig& cfg)
  {
      cfg_ = &cfg;
  }

  /**
   * @brief Set all parameters at once
   * @param cfg TebConfig class
   * @param robot_model Robot model required for distance calculation
   * @param obstacle 2D position vector containing the position of the obstacle
   */ 
  void setParameters(const TebConfig& cfg, const BaseRobotFootprintModel* robot_model, const Obstacle* obstacle)
  {
    cfg_ = &cfg;
    robot_model_ = robot_model;
    _measurement = obstacle;
  }
  
protected:

  const TebConfig* cfg_; //!< Store TebConfig class for parameters
  const BaseRobotFootprintModel* robot_model_; //!< Store pointer to robot_model
  
public: 	
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};
  
    

} // end namespace

#endif
