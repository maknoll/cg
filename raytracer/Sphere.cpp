#include "Sphere.hpp"
#include "Ray.hpp"
#include <memory>

namespace rt
{

std::shared_ptr<RayIntersection>
Sphere::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  //Programming TASK 1: implement this method
  
  real r = 1;
  
  real a = ray.direction().dot(ray.direction());
  real b = 2 * ray.direction().dot(ray.origin());
  real c = ray.origin().dot(ray.origin()) - ( r * r);
  
  //Find discriminant
  real disc = b * b - 4 * a * c;
  
  // if discriminant is negative there are no real roots, so return
  // false as ray misses sphere
  if (disc < 0)
    return false;
  
  // compute q as described above
  real distSqrt = sqrtf(disc);
  real q;
  if (b < 0)
    q = (-b - distSqrt)/2.0;
  else
    q = (-b + distSqrt)/2.0;
  
  // compute t0 and t1
  real t0 = q / a;
  real t1 = c / q;
  
  // make sure t0 is smaller than t1
  if (t0 > t1)
  {
    // if t0 is bigger than t1 swap them around
    real temp = t0;
    t0 = t1;
    t1 = temp;
  }
  
  // if t1 is less than zero, the object is in the ray's negative direction
  // and consequently the ray misses the sphere
  if (t1 < 0)
    return false;
  
  real lambda;
  // if t0 is less than zero, the intersection point is at t1
  if (t0 < 0)
  {
    lambda = t1;
  }
  // else the intersection point is at t0
  else
  {
    lambda = t0;
  }
  
  Vec3 point = ray.origin() + lambda * ray.direction();
  
  Vec3 normal = point - Vec3(0,0,0);
  
  // Only intersections in [0,1] range are valid.
  if (lambda < 0.0 || lambda > maxLambda)
    return nullptr;
  
  return std::make_shared<SphereRayIntersection>(ray,shared_from_this(),
                                                lambda, normal);
}

} //namespace rt
