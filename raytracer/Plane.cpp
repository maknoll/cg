#include "Plane.hpp"
#include "Ray.hpp"
#include "Math.hpp"

namespace rt
{

Plane::Plane(const Vec3& normal) :
  mNormal(normal)
{
  mNormal.normalize();
}

std::shared_ptr<RayIntersection>
Plane::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  // Solve linear equation for lambda
  // see also http://en.wikipedia.org/wiki/Line-plane_intersection
  real a=(-ray.origin()).dot(mNormal);
  real d=ray.direction().dot(mNormal);

  // No intersection if ray is (almost) parallel to plane
  if (fabs(d)<Math::safetyEps())
    return nullptr;

  real lambda = a / d;

  // Only intersections in [0,1] range are valid.
  if (lambda<0.0 || lambda>maxLambda)
    return nullptr;

  return std::make_shared<PlaneRayIntersection>(ray,shared_from_this(),
                                                lambda, mNormal);
}

} //namespace rt
