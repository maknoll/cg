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
  // Möller–Trumbore algorithm
  
  Vec3 edge0, edge1, tvec, pvec, qvec;
  real det, inv_det;
  real u, v;
  
  edge0 = mVertices[1] - mVertices[0];
  edge1 = mVertices[2] - mVertices[0];
  pvec = -util::cross(ray.direction(), edge1);
  
  det = edge0.dot(pvec);
  
  if(det > -Math::safetyEps() && det < Math::safetyEps())
    return nullptr;
  
  inv_det = 1.0 / det;
  
  tvec = ray.origin() - mVertices[0];
  u = tvec.dot(pvec) * inv_det;
  if(u < 0.0 || u > 1.0)
    return nullptr;
  
  qvec = -util::cross(tvec, edge0);
  v = ray.direction().dot(qvec) * inv_det;
  if(v < 0.0 || u + v > 1.0)
    return nullptr;
  
  real lambda = edge1.dot(qvec) * inv_det;
  if(lambda < Math::safetyEps() || lambda > maxLambda)
    return nullptr;
  
  return std::make_shared<TriangleRayIntersection>(ray, lambda,
                                                   shared_from_this(), mNormal);
}

} //namespace rt
