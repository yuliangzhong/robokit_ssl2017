#ifndef _RAY_TRACE_H_
#define _RAY_TRACE_H_

#include <robokit/foundation/quadtree/quadtree_basic.h>

namespace rbk
{
	namespace algorithm
	{
#if defined (__cplusplus)
		extern "C" {
#endif

#ifdef HAVE_SINCOS
#define STG_SINCOS(a, sina, cosa) sincos((a), &(sina), &(cosa));
#else
#define STG_SINCOS(a, sina, cosa) {\
	sina = sin(a); \
	cosa = cos(a); \
}
#endif

#ifndef NORMALIZE
	/// Normalize angle to domain -pi, pi
#define NORMALIZE(z) atan2(sin(z), cos(z))
#endif

  typedef struct
  {
    double x, y, a;
    double cosa, sina, tana;
    double range;
    double max_range;
    double obs_angle; /// If the other model hit is a line edge, this is its angle in world coordinates
    double* incr;

    int index;
    stg_matrix_t* matrix;    
  
  } itl_t;
  
  typedef enum { PointToPoint=0, PointToBearingRange } itl_mode_t;
  
  /** create an itl_t object that can be used for raytracing
   * @param x Starting location X
   * @param y Starting location Y
   * @param a In PointToBearing mode, bearing angle from (x,y) to send the ray.
   * In PointToPoint mode, ending location X?
   * @param b In PointToBearing mode, maximum range to stop the ray. In
   * PointToPoint mode, ending location Y?
   * @param matrix Matrix to search
   * @param pmode Raytracing mode
   */
  itl_t* itl_create( double x, double y, double a, double b, 
		     stg_matrix_t* matrix, itl_mode_t pmode );

  /** Reset members of an existing itl_t object. Use to avoid reallocating
   * objects in a loop. */
  itl_t* itl_reset(itl_t* itl, double x, double y, double a, double b, itl_mode_t pmode);
  
  void itl_destroy( itl_t* itl );

double RayLength(stg_matrix_t* matrix, double start_x, double start_y, double start_theta, double max_range);
#if defined (__cplusplus)
		}
#endif
	}
}
#endif	// ~_RAY_TRACE_H_
