#ifndef PLANE_HPP_INCLUDE_ONCE
#define PLANE_HPP_INCLUDE_ONCE

#include <memory>

#include "Renderable.hpp"
#include "Ray.hpp"

namespace rt
{

/// Infinite plane defined by point on plane and plane normal.
class Plane : public Renderable
{
public:
  Plane(const Vec3& normal = Vec3(0.0,0.0,1.0));

  /// Implements the intersection computation between ray and plane
  virtual std::shared_ptr<RayIntersection>
  closestIntersectionModel(const Ray &ray, real maxLambda) const override;

  const Vec3& normal() const { return mNormal; }

  void setNormal(const Vec3 &normal ) { mNormal=normal; mNormal.normalize(); }

private:
  Vec3 mNormal, mTangent, mBitangent;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Intersection between plane and ray.
class PlaneRayIntersection : public RayIntersection
{
public:
  PlaneRayIntersection(const Ray &ray, const real lambda,
                       std::shared_ptr<const Renderable> renderable,
                       const Vec3 &normal, const Vec3 &uvw) :
    RayIntersection(ray,renderable,lambda,normal,uvw) {}
};

}
#endif //PLANE_HPP_INCLUDE_ONCE
