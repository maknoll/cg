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
}

std::shared_ptr<RayIntersection>
Triangle::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  //Programming TASK 1: implement this method

  return nullptr;
}

} //namespace rt
