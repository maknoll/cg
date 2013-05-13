#include "Sphere.hpp"
#include "Ray.hpp"
#include <memory>

namespace rt
{

std::shared_ptr<RayIntersection>
Sphere::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  // Implicit sphere: (x-c)^2-r^2 = 0
  // r = 1, c=0
  // x^2 = 1
  // Ray: x = o+t*d
  
  // Solve: 0=(o+t*d)^2 -1
  // 0 = t^2*(d*d) + t*(2*o*d) + (o*o-1)

  // Then, these are the 2 possible solutions
  // t0 = 0.5*(-b-sqrt(b*b - 4*a))

  // simplify
  // t01 = -o.d +- sqrt( (o.d)^2 - o.o + 1)

  const Vec3 &d = ray.direction();
  const Vec3 &o = ray.origin();

  real b = util::dot(o,d);
  real c = o.normSquared() - 1;

  real ds = b*b - c;

  //discriminant is negative, no intersection
  if(ds < 0)
    return nullptr;

  real dssqr = sqrt(ds);

  real t0 = -b-dssqr;
  real t1 = -b+dssqr;

  real lambda = t0;

  //ray starts inside sphere, discard first hit
  if(t0 < 0)
    lambda = t1;

  if(lambda < 0 || lambda > maxLambda)
    return nullptr;

  // Compute intersection point
  const Vec3 p = ray.pointOnRay(lambda);

  // Compute parameterization of intersection point
  const real theta = std::atan2(p[1], p[0]);
  const real phi   = std::acos (p[2]);
  const Vec3 uvw(theta,phi,real(0));

  return std::make_shared<SphereRayIntersection>(ray, lambda,
    shared_from_this(), ray.pointOnRay(lambda),uvw);
}

} //namespace rt
