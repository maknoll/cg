#ifndef _SPHERE_HPP_INCLUDE_ONCE
#define _SPHERE_HPP_INCLUDE_ONCE

#include "Renderable.hpp"
#include "Ray.hpp"
#include "Material.hpp"

namespace rt
{

class SphereRayIntersection;

//Unit sphere; i.e. sphere centered at origin with radius = 1
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
  SphereRayIntersection(const Ray &ray,  const real lambda,
                        std::shared_ptr<const Renderable> renderable,
                        const Vec3 &normal, const Vec3 &uvw) :
    RayIntersection(ray,renderable,lambda,normal,uvw) {}
};

} //namespace rt

#endif //_SPHERE_HPP_INCLUDE_ONCE
