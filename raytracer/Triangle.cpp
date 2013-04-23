#include "Triangle.hpp"
#include "Ray.hpp"
#include <memory>

namespace rt
{
Triangle::Triangle(const Vec3 &vertex0, const Vec3 &vertex1, const Vec3 &vertex2)
{
  mVertices[0] = vertex0;
  mVertices[1] = vertex1;
  mVertices[2] = vertex2;
  Vec3 edge0 = vertex1 - vertex0;
  Vec3 edge1 = vertex2 - vertex0;
  Vec3 normal = -util::cross(edge0, edge1).normalized();
  mNormal = normal;
}

std::shared_ptr<RayIntersection>
Triangle::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  return nullptr;
  real a=(-ray.origin()).dot(mNormal);
  real d=ray.direction().dot(mNormal);
  
  // No intersection if ray is (almost) parallel to plane
  if (fabs(d)<Math::safetyEps())
    return nullptr;
  
  real lambda = a / d;
  
  // Only intersections in [0,1] range are valid.
  if (lambda < 0.0 || lambda > maxLambda)
    return nullptr;
  
  //real p = ()
  
  return std::make_shared<TriangleRayIntersection>(ray, lambda,
                                                   shared_from_this(), mNormal);
}

} //namespace rt
