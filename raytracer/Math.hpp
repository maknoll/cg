#ifndef MATH_HPP_INCLUDE_ONCE
#define MATH_HPP_INCLUDE_ONCE

#include <cmath>

#include "Vector.hpp"
#include "Matrix.hpp"

#ifndef M_PI
# define M_PI 3.14159265358979323846 /* pi */
# define M_PI_2 1.57079632679489661923 /* pi/2 */
# define M_PI_4 0.78539816339744830962 /* pi/4 */
# define M_1_PI 0.31830988618379067154 /* 1/pi */
# define M_2_PI 0.63661977236758134308 /* 2/pi */
# define M_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(pi) */
# define M_PIl 3.1415926535897932384626433832795029L /* pi */
# define M_PI_2l 1.5707963267948966192313216916397514L /* pi/2 */
# define M_PI_4l 0.7853981633974483096156608458198757L /* pi/4 */
# define M_1_PIl 0.3183098861837906715377675267450287L /* 1/pi */
# define M_2_PIl 0.6366197723675813430755350534900574L /* 2/pi */
# define M_2_SQRTPIl 1.1283791670955125738961589031215452L /* 2/sqrt(pi) */
#endif

namespace rt
{

typedef double real;
typedef util::Vector<2,real> Vec2;
typedef util::Vector<3,real> Vec3;
typedef util::Vector<4,real> Vec4;

typedef util::AffineMatrix<4,real> Mat4;

typedef util::Vector<2,float> Vec2f;
typedef util::Vector<3,float> Vec3f;
typedef util::Vector<4,float> Vec4f;

typedef util::Vector<2,double> Vec2d;
typedef util::Vector<3,double> Vec3d;
typedef util::Vector<4,double> Vec4d;

typedef util::Vector<3,int> Vec2i;
typedef util::Vector<3,int> Vec3i;
typedef util::Vector<3,int> Vec4i;

class Math
{
public:

  template <typename T>
  static T clamp(T val, T minVal=T(0), T maxVal=T(1))
  {
    return std::max(minVal, std::min(maxVal,val));
  }

  inline static const real safetyEps() { return 0.000000001; }

  template <typename T>
  static T log2(T d) {return log(d)/log(T(2)) ;}	
};

} //namespace rt

#endif //MATH_HPP_INCLUDE_ONCE
