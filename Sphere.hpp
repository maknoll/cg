#ifndef _SPHERE_HPP_INCLUDE_ONCE
#define _SPHERE_HPP_INCLUDE_ONCE

#include "Renderable.hpp"
#include "Ray.hpp"
#include "Material.hpp"

namespace rt
{

class SphereRayIntersection;

/// Sphere defined by position and radius.
class Sphere : public Renderable
{
public:
  /// Implements the intersection computation between ray and sphere.
  std::shared_ptr<RayIntersection>
  closestIntersectionModel(const Ray &ray, real maxLambda) const override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Intersection between sphere and ray.
class SphereRayIntersection : public RayIntersection
{
public:
  SphereRayIntersection(const Ray &ray,
                        std::shared_ptr<const Renderable> renderable,
                        const real lambda,
                        const Vec3 &normal) :
    RayIntersection(ray,renderable,lambda,normal) {}
};

} //namespace rt

#endif //_SPHERE_HPP_INCLUDE_ONCE
