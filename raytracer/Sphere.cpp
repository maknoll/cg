#include "Sphere.hpp"
#include "Ray.hpp"
#include <memory>

namespace rt
{

std::shared_ptr<RayIntersection>
Sphere::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  //Programming TASK 1: implement this method
  
  real a = ray.direction().dot(ray.direction());
  real b = 2 * ray.direction().dot(ray.origin());
  real c = ray.origin().dot(ray.origin()) - 1;

  real disc = b * b - 4 * a * c;
  
  if (disc < 0)
    return false;
  
  real dist_sqrt = sqrtf(disc);
  real q;
  if (b < 0)
    q = (-b - dist_sqrt)/2.0;
  else
    q = (-b + dist_sqrt)/2.0;
  
  real t0 = q / a;
  real t1 = c / q;
  
  if (t0 > t1)
  {
    real temp = t0;
    t0 = t1;
    t1 = temp;
  }
  
  if (t1 < 0)
    return false;
  
  real lambda;
  
  if (t0 < 0)
  {
    lambda = t1;
  }

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
