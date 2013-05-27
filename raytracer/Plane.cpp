#include "Plane.hpp"
#include "Ray.hpp"
#include "Math.hpp"

namespace rt
{

Plane::Plane(const Vec3& normal) :
  mNormal(normal)
{
  mNormal.normalize();

  mTangent = Vec3(1,0,0);

  mBitangent = mNormal % mTangent;

  const real bitangentNorm = mBitangent.norm();

  if(bitangentNorm > Math::safetyEps())
  {
    mBitangent /= bitangentNorm;
    mTangent = (mBitangent % mNormal).normalize();
  } else
  {
    mTangent = Vec3(0,1,0);
    mBitangent = Vec3(0,0,1);
  }
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

  const Vec3 p = ray.pointOnRay(lambda);
  const Vec3 uvw(p | mTangent, p | mBitangent, real(0));

  return std::make_shared<PlaneRayIntersection>(ray,lambda, shared_from_this(),
    mNormal,uvw);
}

BoundingBox Plane::computeBoundingBox() const
{
  return BoundingBox(Vec3(-std::numeric_limits<real>::infinity(),
                          -std::numeric_limits<real>::infinity(),
                          -std::numeric_limits<real>::infinity()),
                     Vec3( std::numeric_limits<real>::infinity(),
                           std::numeric_limits<real>::infinity(),
                           std::numeric_limits<real>::infinity()));
}

} //namespace rt
