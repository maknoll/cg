#ifndef MATH_HPP_INCLUDE_ONCE
#define MATH_HPP_INCLUDE_ONCE

#include <cmath>
#include <algorithm>
#include <limits>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix2.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include <random>


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

namespace ogl
{
typedef vl::Vector2<float> Vec2;
typedef vl::Vector3<float> Vec3;
typedef vl::Vector4<float> Vec4;

typedef vl::Matrix3<float> Mat3;
typedef vl::Matrix4<float> Mat4;

typedef vl::Vector2<int> Vec2i;
typedef vl::Vector3<int> Vec3i;
typedef vl::Vector4<int> Vec4i;

/*
inline static Vec3 cross (const Vec3 &a, const Vec3 &b)
{
  return vl::cross(a,b);
}

inline static float dot(const Vec3& a, const Vec3 &b)
{
  return vl::dot(a,b);
}
*/

class Math
{
public:

  template <typename T>
  static T clamp(T val, T minVal=T(0), T maxVal=T(1))
  {
    return std::max(minVal, std::min(maxVal,val));
  }

  inline static const float safetyEps() { return float(0.0001f); }



  template <typename T>
  static T log2(T d) {return log(d)/log(T(2)) ;}


  static void orthonormalBasis(const Vec3& normal, Vec3& U, Vec3& V, Vec3& W)
  {
    // create orthonormal basis
    W = normal;
    W.normalize();
    U = cross(W, Vec3(0,1,0));
    if (fabs(U[0]) < Math::safetyEps() && fabs(U[1]) < Math::safetyEps() && fabs(U[2]) < Math::safetyEps())
      U = cross(W, Vec3(1,0,0));
    U.normalize();
    V = cross(W,U);
  }

  static Vec3 sampleUnitSphere()
  {
    float phi = random(0,float(2*M_PI));
    float r = random(); r=r*r*r;
    float cos_theta = random(-1,1);

    float x = r*sqrt(1-cos_theta*cos_theta)*cos(phi);
    float y = r*sqrt(1-cos_theta*cos_theta)*sin(phi);
    float z = r*cos_theta;

    return Vec3(x,y,z).normalize();
  }

  static Vec3 sampleUnitHemisphereCosine()
  {
    float sample_x = random();
    float sample_y = random();
    float phi = float(2.f * M_PI * sample_x);
    float r = float(sqrt(sample_y));
    float x = r * float(cos(phi));
    float y = r * float(sin(phi));
    float z = 1.f - x*x - y*y;
    z = z > 0.f ? sqrt(z) : 0.f;
    return Vec3(x,y,z);
  }

  static Vec3 sampleDirectionUnitHemisphereCosine(const Vec3 &N)
  {
    Vec3 U,V,W;
    orthonormalBasis(N,U,V,W);

    Vec3 D = sampleUnitHemisphereCosine();

    return (U * D[0] + V * D[1] + W * D[2]).normalize();
  }

  static Vec3 samplePhongLobe(const float exponent)
  {
   float sample_x = random();
   float sample_y = random();
   float power = exp( log(sample_y) / (exponent+1.f) );
   float phi = float(2.f * M_PI * sample_x);
   float scale = sqrt(1.f-power*power);
   float x = scale * float(cos(phi));
   float y = scale * float(sin(phi));
   float z = power;
   return Vec3(x,y,z);
  }

  static Vec3 sampleDirectionPhongLobe(const Vec3 & N, const float exponent)
  {
    Vec3 U,V,W;
    orthonormalBasis(N,U,V,W);

    Vec3 D = samplePhongLobe(exponent);

    return (U * D[0] + V * D[1] + W * D[2]).normalize();
  }

  // generates a uniformly distributed random number in [0..1]
  inline static const float random(float minValue = 0.f, float maxValue = 1.f)
  {
    return mUniformDistribution(mRandomGenerator) * (maxValue-minValue)+minValue;
  }

private:
  static std::default_random_engine mRandomGenerator;
  static std::uniform_real_distribution<float> mUniformDistribution;
};

} //namespace rt

#endif //MATH_HPP_INCLUDE_ONCE
