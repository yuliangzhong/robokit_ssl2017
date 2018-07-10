#include <iostream>
#include <math.h>
#include <stdio.h>
#include <CMmotion.h>
#include <utils.h>

const double FRAME_PERIOD = 1.0 / Param::Vision::FRAME_RATE;
//��֪��������ʲô��һ����������ע��by zhy 2010.11.21
//float motion_time_1d(float dx,float vel0,float vel1,
//                            float max_vel,float max_accel,
//                            float &t_accel,float &t_cruise,float &t_decel){
//  float tmp,vmax;
//  const bool debug = true;
//  if(dx < 0){
//    dx = -dx;
//    vel0 = -vel0;
//    vel1 = -vel1;
//  }
//  if(debug){
//    printf("  d%f v%f-%f m%f a%f = ",
//           dx,vel0,vel1,max_vel,max_accel);
//  }
//  if(vel0 > max_vel) vel0 = max_vel;
//  if(vel1 > max_vel) vel1 = max_vel;
//
//  // stop
//  if(vel0 > vel1){
//    t_decel = (vel0 + vel1) / 2.0*dx;  //----->����Ҫ��ʲô��
//    if(debug) printf("<%f> ",fabs(vel1 - vel0)/t_decel);
//    if(fabs(vel1 - vel0)/t_decel > max_accel){
//      t_accel = t_cruise = 0;
//      if(debug) printf("\n");
//      return(t_decel);
//    }
//  }//���ٶȲ���
//
//  // calculate time spent at max velocity
//  tmp = 2*max_accel*dx + vel0*vel0 + vel1*vel1;
//  t_cruise = (tmp - 2*max_vel*max_vel) / (2*max_vel*max_accel);//�����˶�ʱ��
//
//  if(t_cruise > 0){
//    vmax = max_vel;//�ܴﵽ����ٶ�
//  }else{
//    vmax = sqrt((max_accel*dx + vel0*vel0 + vel1*vel1)/2);
//    t_cruise = 0;//�ﲻ������ٶȣ����ٵ�ĳһ�ٶȺ����
//  }
//
//  if(debug) printf("[am%f] ",vmax);
//
//  t_accel = max(vmax - vel0,(float)0.0) / max_accel;//����ʱ��
//  t_decel = fabs(vmax - vel1) / max_accel;//����ʱ��
//
//  if(debug){
//    printf("t(%f,%f,%f):%f\n",
//           t_accel,t_cruise,t_decel,t_accel+t_cruise+t_decel);
//  }
//
//  return(t_accel + t_cruise + t_decel);//�ܵ�ʱ��
//}


 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// @fn	void compute_motion_1d(double x0, double v0, double v1, double a_max, double v_max,
 /// 		double a_factor, double &traj_accel, double &traj_time)
 ///
 /// @brief	һά״̬�¼����˶����̣�������ʼ�ٶȺ�ĩ�ٶȣ������ٶȺ�����ٶȣ����ؼ���ʱ��͹���ʱ��
 ///
 /// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
 /// @date		2010-4-2
 ///
 /// @param	x0				��ʼλ��
 /// @param	v0				���ٶ�
 /// @param	v1				ĩ�ٶ�
 /// @param	a_max			�����ٶ�
 /// @param	v_max			����ٶ�
 /// @param	a_factor		������Ӱ����ٶȵ�����
 /// @param	traj_accel		����ü��ٶ�
 /// @param	traj_time		����ü���ʱ��
 ////////////////////////////////////////////////////////////////////////////////////////////////////

void compute_motion_1d(double x0, double v0, double v1,
    double a_max, double d_max, double v_max, double a_factor,
    double &traj_accel, double &traj_time)
{
    if (x0 == 0 && v0 == v1)
    { 
        traj_accel = 0;
        traj_time = 0;
        return; 
    }

    if(!finite(x0) || !finite(v0) || !finite(v1))
    {
        traj_accel = 0; 
        traj_time = 0;  
        return;
    }

    a_max /= a_factor;
		d_max /= a_factor;

    double accel_time_to_v1 = fabs(v1 - v0) / a_max; // �����ٶȼӼ��ٵ�ĩ�ٶȵ�ʱ��
    double accel_dist_to_v1 = fabs((v1 + v0) / 2.0) * accel_time_to_v1; // ��һ���ٵ�ĩ�ٶ�ʱ��λ��
    double decel_time_to_v1 = fabs(v0 - v1) / d_max; // �����ٶȼ��ٵ�ĩ�ٶȵ�ʱ��
    double decel_dist_to_v1 = fabs((v0 + v1) / 2.0) * decel_time_to_v1; // ��һ���ٵ�ĩ�ٶ�ʱ��λ��

    // ���ʱ��ܹؼ�����ýϴ���λ���Ƚ���󽵵� by qxz
    double period = 1.0 / 40.0; // һ�κ�С��ʱ�䣬�����˶���Ŀ��㸽��ʱ���ٶȣ��ȶ����㣬��ֹ����

    v1 = _copysign(v1, -x0); // ����ת�� ��v1����ת��Ϊλ�Ƶķ���

    // ��x0�˶������
    // ���ٺ�Ŀ��㷴�� �� ���ٴ���Ŀ�������������ٵ�Ŀ����
    // ȫ�̼���
    if (v0 * x0 > 0 || (fabs(v0) > fabs(v1) && decel_dist_to_v1 > fabs(x0))) 
    {
        // ͣ�º󵽴��ʱ�� + ͣ������ʱ��
        double time_to_stop = fabs(v0) / d_max; // ͣ��ʱ��
        double x_to_stop = v0 * v0 / (2.0 * d_max); // ֹͣʱ�˶�����

        compute_motion_1d(x0 + _copysign(x_to_stop, v0), 0, v1, a_max * a_factor, d_max * a_factor,
            v_max, a_factor, traj_accel, traj_time); // �ݹ�����ֱ��������һ����
        traj_time += time_to_stop; // ·���滮ʱ�����ȥ

        // ���� 
        if (traj_time < period)
            traj_accel = compute_stop(v0, d_max * a_factor); // �ܿ��Ҫ���ˣ�����һ�����ʵļ��ٶ�
        else if (time_to_stop < period)
            // ��������ʹ�ü��ٶ���һ���ɴ�С�Ľ���
            traj_accel = time_to_stop / period * - _copysign(d_max * a_factor, v0) + (1.0 - time_to_stop / period) * traj_accel;
        else
            traj_accel = -_copysign(d_max * a_factor, v0);

        return;
    }

    // At this point we have two options.  We can maximally accelerate
    // and then maximally decelerate to hit the target.  Or we could
    // find a single acceleration that would reach the target with zero
    // velocity.  The later is useful when we are close to the target
    // where the former is less stable.

    // [[ OPTION 1
    // This computes the amount of time to accelerate before decelerating.
    double t_a, t_accel, t_decel;

    // ���ٺ�Ŀ���ͬ��
    if (fabs(v0) > fabs(v1))
        // ���ٶȴ���Ŀ���٣������Լ��ٵ�Ŀ����
        // �ȼ����ټ���
    {
        t_a = (sqrt((d_max * v0 * v0 + a_max * (v1 * v1 + 2 * d_max * fabs(x0)))
            / (a_max + d_max)) - fabs(v0)) / a_max; //���ٶȴ���ĩ�ٶȣ����ٵ�ʱ��

        if (t_a < 0.0)
            t_a = 0;
        t_accel = t_a;
        t_decel = ((fabs(v0) - fabs(v1)) + a_max * t_a) / d_max;
    } 

    else if (accel_dist_to_v1 > fabs(x0)) 
        // ���ٶ�С��Ŀ���٣��Ҳ��ɼ��ٵ�Ŀ����
        // ȫ�̼���
    {
        t_a = (sqrt(v0 * v0 + 2 * a_max * fabs(x0)) - fabs(v0)) / a_max;
        t_accel = t_a;
        t_decel = 0.0;//ֱ�������ٶȼ��ٵ�ָ���ٶȻᳬ��Ŀ��㣬���ٲ���ָ��ĩ�ٶ�
    }

    else
        // ���ٶ�С��Ŀ���٣��ҿ��Լ��ٵ�Ŀ����
        // �ȼ����ټ���
    {
        t_a = (sqrt((d_max * v0 * v0 + a_max * (v1 * v1 + 2 * d_max * fabs(x0)))
            / (a_max + d_max)) - fabs(v0)) / a_max; //���ٶȴ���ĩ�ٶȣ����ٵ�ʱ��

        if (t_a < 0.0)
            t_a = 0;
        t_accel = t_a;
        t_decel = ((fabs(v0) - fabs(v1)) + a_max * t_a) / d_max;
    }
    // ]] OPTION 1

    // [[ OPTION 2
    double a_to_v1_at_x0 = (v0 * v0 - v1 * v1) / (2 * fabs(x0));//���ʵļ��ٶ�
    double t_to_v1_at_x0 =
        (-fabs(v0) + sqrt(v0 * v0 + 2 * fabs(a_to_v1_at_x0) * fabs(x0))) /
        fabs(a_to_v1_at_x0);//��ʱ�����ʱ��

    // We follow OPTION 2 if t_a is less than a FRAME_PERIOD making it
    // difficult to transition to decelerating and stopping exactly.
    if (0 && a_to_v1_at_x0 < a_max && a_to_v1_at_x0 > 0.0 &&
        t_to_v1_at_x0 < 2.0 * FRAME_PERIOD && 0) 
    {
        // OPTION 2
        // Use option 1 time, even though we're not following it.
        traj_time = t_accel + t_decel;;

        // Target acceleration to stop at x0.
        traj_accel = -_copysign(a_to_v1_at_x0, v0);

        return;
    } 
    // ]] OPTION 2

    else
    {
        //printf("option 1");
        // OPTION 1
        // ����ʱ�� +������ʱ��
        traj_time = t_accel + t_decel;

        // �������ó��ٿ��ܳ���������ٶȣ�����һ�������˶�
        if (t_accel * a_max + fabs(v0) > v_max)
        {
            // �����˶���ʱ��
					traj_time += (pow(a_max * t_accel + fabs(v0), 2.0) - pow(v_max, 2.0)) / (2 * a_max) / v_max;
					traj_time += (pow(d_max * t_decel + fabs(v1), 2.0) - pow(v_max, 2.0)) / (2 * d_max) / v_max;
        }

        // Accelerate (unless t_accel is less than FRAME_PERIOD, then set
        // acceleration to average acceleration over the period.)
        if (t_accel < period && t_decel == 0.0)
        {
            traj_accel = _copysign(a_max * a_factor, -x0);
        }
        else if (t_accel < period && t_decel > 0.0)
        {
            traj_accel = compute_stop(v0, d_max * a_factor);
        }
        else if (t_accel < period) 
        {
            traj_accel = _copysign((2.0 * t_accel / (period) - 1) * a_max * a_factor, v0);
        }
        else
				{
            traj_accel = _copysign(a_max * a_factor, -x0);
        }
    }
}


 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// @fn	void compute_motion_2d(CVector x0, CVector v0, CVector v1, double a_max, double v_max,
 /// 		double a_factor, CVector &traj_accel, double &time)
 ///
 /// @brief �ڶ�ά�¼����˶����̣�������ʼ�ٶ������������ٶ��������Լ������ٶȣ�����ٶ�
 ///
 /// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
 /// @date		2010-4-2
 ///
 /// @param	x0						initial coordinate
 /// @param	v0						initial velocities
 /// @param	v1						final velocities 
 /// @param	a_max					maxium of accelerate
 /// @param	v_max					maxium of velocities
 /// @param	a_factor				factor of accelerate
 /// @param [in,out]	traj_accel	the traj accel. 
 /// @param [in,out]	time		the time. 
 ////////////////////////////////////////////////////////////////////////////////////////////////////

void compute_motion_2d(CVector x0, CVector v0, CVector v1,
                    double a_max, double d_max, double v_max, 
                    double a_factor, CVector &traj_accel, double &time)
{
  double time_x;
  double time_y;
  double rotangle;
  double traj_accel_x;
  double traj_accel_y;
  if (v1.mod() > 0.0)
      rotangle = v1.dir();
  else
      rotangle = 0.0;

  x0 = x0.rotate(-rotangle);
  v0 = v0.rotate(-rotangle);
  v1 = v1.rotate(-rotangle); //����ϵת����ת����ĩ�ٶȷ���Ϊx�������ϵ��

  compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max, d_max, v_max, a_factor,
		    traj_accel_x, time_x);
  compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max, d_max, v_max, a_factor,
		    traj_accel_y, time_y);//����ͬ��������ٶȡ����ٶȶ��������������˶�ʱ��

  if (v1.mod() < 0.01)
  {
    double rx = time_x / hypot(time_x, time_y);
    double ry = time_y / hypot(time_x, time_y);
    double rx_a = rx;
	double rx_v = rx;
	//����������ܱ�֤x�ᡢy���˶�ͬ������������������
	if (time_x < time_y * 1.5) 
    {
		rx_a = pow(time_x / (time_y * 1.5), 2.0);
		rx_v = time_x / (time_y * 1.5);
	}
    compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max * rx_a, d_max * rx_a, v_max * rx_v, a_factor,
		      traj_accel_x, time_x);
    compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max * ry, d_max * ry, v_max * ry, a_factor,
		      traj_accel_y, time_y);

	////////////////////////////////////////////////////////////////////////////
	///*
	//*	modified by yph at 2010-6-5�� ���ж��ֵ���
	//*/

	////std::cout<<"Zero-Scale-before: x/y = "<<rx/ry<<std::endl;
	//rx = time_x / hypot(time_x, time_y);
	//ry = time_y / hypot(time_x, time_y);
	//const int MAX_ITERATE_NUM = 4;
	//for(int iterater = 0; iterater < MAX_ITERATE_NUM; iterater++)
	//{
	//	compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max * rx, v_max * rx, a_factor,
	//			traj_accel_x, time_x);
	//	compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max * ry, v_max * ry, a_factor,
	//			traj_accel_y, time_y);

	//	rx = time_x / hypot(time_x, time_y);
	//	ry = time_y / hypot(time_x, time_y);

	//	if( fabs((rx/ry)-1.0) < 0.1 )
	//		break;
	//}
	//std::cout<<"Zero-Scale-after: x/y = "<<rx/ry<<std::endl;

  } // if (v1.mod() < 0.01)
  else
  {
	// �����ٶȵ���
    if (time_x < time_y * 1.5) 
    {
      double rx_a = pow(time_x / (time_y * 1.5), 2.0);
      double rx_v = time_x / (time_y * 1.5);
      // x�����ٴδ�������ٶ������ٶȳ���һ˥�����ӣ������⣿��������������ͬ���ˣ�����
      compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max * rx_a, d_max * rx_a, v_max * rx_v, a_factor,
			traj_accel_x, time_x);
    }

	/*
	*	modified by yph at 2010-6-5�� ���ж��ֵ���
	*/
	//double rx = time_x / hypot(time_x, time_y);
	//double ry = time_y / hypot(time_x, time_y);
	//const int MAX_ITERATE_NUM = 5;
	//for(int iterater = 0; iterater < MAX_ITERATE_NUM; iterater++)
	//{
	//	compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max * rx, v_max * rx, a_factor,
	//		traj_accel_x, time_x);
	//	compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max * ry, v_max * ry, a_factor,
	//		traj_accel_y, time_y);

	//	rx = time_x / hypot(time_x, time_y);
	//	ry = time_y / hypot(time_x, time_y);

	//	if( rx / ry >= 1.25 && rx / ry <= 20 )	//���ȿ���y�᷽�򣬱�֤y���ȵ���
	//		break;
	//}
  }
  
  traj_accel = CVector(traj_accel_x,traj_accel_y);
  traj_accel = traj_accel.rotate(rotangle);
  time = std::max(time_x,time_y);
  //std::cout<<"time:"<<time<<"traj_accel:"<<traj_accel.mod()<<std::endl;
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// @fn	double compute_stop(double v, double max_a)
  ///
  /// @brief	Calculates the stop. 
  ///
  /// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
  /// @date		2010-4-2
  ///
  /// @param	v		The v. 
  /// @param	max_a	The maximum a. 
  ///
  /// @return	The calculated stop. 
  ////////////////////////////////////////////////////////////////////////////////////////////////////

double compute_stop(double v, double max_a)
{
  if (fabs(v) > max_a * FRAME_PERIOD) return _copysign(max_a, -v); // ���һ֡�ڲ��ܼ��ٵ�0�����������ٶ�
  else return -v / FRAME_PERIOD; // ����ܼ���0�����ٶȾ����ٶȳ���һ֡��ʱ�伴��
}

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// @fn	void goto_point_omni( const PlayerPoseT& start, const PlayerPoseT& final,
  /// 		const PlayerCapabilityT& capability, const double& accel_factor,
  /// 		const double& angle_accel_factor, PlayerPoseT& nextStep)
  ///
  /// @brief	������ʼ���꣬�������꼰С�����ܣ�����С�����ܵĹ켣��������һ��λ�õĻ�����״̬
  ///
  /// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
  /// @date		2010-4-2
  ///
  /// @param	start				The start. 
  /// @param	final				The final. 
  /// @param	capability			The capability. 
  /// @param	accel_factor		The accel factor. 
  /// @param	angle_accel_factor	The angle accel factor. 
  /// @param [in,out]	nextStep	the next step. 
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  void goto_point_omni( const PlayerPoseT& start,
	  const PlayerPoseT& final,
	  const PlayerCapabilityT& capability,
	  const double& accel_factor,
	  const double& angle_accel_factor,
	  PlayerPoseT& nextStep)
  {
	  CGeoPoint target_pos = final.Pos();
	  CVector x = start.Pos() - target_pos;
	  CVector v = start.Vel();
	  double ang = Utils::Normalize(start.Dir() - final.Dir());
	  double ang_v = start.RotVel();
      CVector target_vel = final.Vel();
	  double max_accel = capability.maxAccel;
      double max_decel = capability.maxDec;
	  double max_speed = capability.maxSpeed;
	  double max_angle_speed = capability.maxAngularSpeed;
	  double max_angle_accel = capability.maxAngularAccel;
      double max_angle_decel = capability.maxAngularDec;
	  CVector a;    
	  double ang_a, factor_a;
	  double time_a, time;
	  compute_motion_2d(x, v, target_vel, 
		  max_accel, 
          max_decel,
		  max_speed,
		 accel_factor,
		  a, time);
	  factor_a = 0.5 - 0.1;
	  //�õ������Ƚ�factor_a
	  do{
		  factor_a += 0.1;
		  compute_motion_1d(ang, ang_v, 0.0,
			  max_angle_accel*factor_a,
              max_angle_decel*factor_a,
			  max_angle_speed,
			 angle_accel_factor,
			  ang_a, time_a);
	  }while(factor_a<1.0 && time_a>time);
	  // printf("factor_a = %f\n",factor_a);

	  v = v + a * FRAME_PERIOD;
	  ang_v += ang_a * FRAME_PERIOD;
	  if (v.mod() > max_speed)
		  v = v* max_speed/v.mod();
	  if (ang_v>max_angle_speed)
	  {
		  ang_v = max_angle_speed;
	  }
      else if (ang_v<-max_angle_speed)
	  {
		  ang_v = -max_angle_speed;
	  }
	  CGeoPoint next_pos = start.Pos()+Utils::Polar2Vector(v.mod()*FRAME_PERIOD,v.dir());
	  double next_angle = start.Dir()+ang_v*FRAME_PERIOD;
      double t =  std::max(time,time_a);
	  nextStep.SetValid(true);
	  nextStep.SetPos(next_pos);
	  nextStep.SetDir(next_angle);
	  nextStep.SetVel(v);
	  nextStep.SetRotVel(ang_v);
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	double expectedCMPathTime(const PlayerPoseT& start, const CGeoPoint& final,
/// 	double maxAccel, double maxVelocity, double accel_factor)
///
/// @brief	���� compute_motion_2d
///
/// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
/// @date	2010-4-2
///
/// @param	start			The start. 
/// @param	final			The final. 
/// @param	maxAccel		The maximum accel. 
/// @param	maxVelocity		The maximum velocity. 
/// @param	accel_factor	The accel factor. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////

double expectedCMPathTime(const PlayerPoseT& start, const CGeoPoint& final, double maxAccel, double maxVelocity, double accel_factor)
{
	CGeoPoint target_pos = final;
	CVector x = start.Pos() - target_pos;
	CVector v = start.Vel();
	CVector target_vel = CVector(0,0);
	CVector a;  
	double time;
	compute_motion_2d(x, v, target_vel, 
		maxAccel, 
        maxAccel,
		maxVelocity,
		accel_factor,
		a, time);
    return time;
}
