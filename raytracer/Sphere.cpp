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
	
	if (disc < 0) return nullptr;
	
	real distance = sqrtf(disc);
	real q = b < 0 ? (-b - distance) / 2.0 : (-b + distance) / 2.0;
	
	real t0 = q / a;
	real t1 = c / q;
	
	real lambda = t0 < 0 ? t1 : t0;
	
	Vec3 normal = ray.origin() + lambda * ray.direction();
	
	// Only intersections in [0,1] range are valid.
	if (lambda < 0.0 || lambda > maxLambda)
	  return nullptr;
	
	return std::make_shared<SphereRayIntersection>(ray,shared_from_this(),
												   lambda, normal);
  }
  
} //namespace rt
