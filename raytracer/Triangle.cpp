#include "Triangle.hpp"
#include "Ray.hpp"
#include <memory>
#include "Intersection.hpp"

namespace rt
{
Triangle::Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                   const Vec3 &uvw0, const Vec3 &uvw1, const Vec3 &uvw2)
{
  mVertices[0] = v0;
  mVertices[1] = v1;
  mVertices[2] = v2;
  mUVW[0] = uvw0;
  mUVW[1] = uvw1;
  mUVW[2] = uvw2;
}

std::shared_ptr<RayIntersection>
Triangle::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  Vec3 bary;
  real lambda;

  if(!Intersection::lineTriangle(ray,mVertices[0],mVertices[1],mVertices[2],bary,lambda))
    return nullptr;

  // Intersection is inside triangle if 0<=u,v,w<=1
  if(lambda<0 || lambda>maxLambda)
    return nullptr;

  const Vec3 normal = util::cross(mVertices[1]-mVertices[0],mVertices[2]-mVertices[0]).normalized();
  const Vec3 uvw = mUVW[0]*bary[0]+mUVW[1]*bary[1]+mUVW[2]*bary[2];

    return std::make_shared<TriangleRayIntersection>(ray, lambda, shared_from_this(),normal,uvw);
                                
}

} //namespace rt
