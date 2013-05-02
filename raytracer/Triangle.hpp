#ifndef _TRIANGLE_HPP_INCLUDE_ONCE
#define _TRIANGLE_HPP_INCLUDE_ONCE

#include "Renderable.hpp"
#include "Ray.hpp"
#include "Material.hpp"

namespace rt
{

class TriangleRayIntersection;

/// Triangle defined by position and radius.
class Triangle : public Renderable
{
public:
  Triangle( const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
            const Vec3 &uvw0=Vec3(0,0,0), const Vec3 &uvw1=Vec3(0,0,0),
            const Vec3 &uvw2=Vec3(0,0,0));

  /// Implements the intersection computation between ray and Triangle.
  std::shared_ptr<RayIntersection>
  closestIntersectionModel(const Ray &ray, real maxLambda) const override;

private:

  // Vertex positions
  Vec3 mVertices[3];
  Vec3 mUVW[3];
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Intersection between Triangle and ray.
class TriangleRayIntersection : public RayIntersection
{
public:
  TriangleRayIntersection(const Ray &ray, const real lambda,
                          std::shared_ptr<const Renderable> renderable,
                          const Vec3 &normal, const Vec3 &uvw) :
    RayIntersection(ray,renderable,lambda,normal,uvw) {}
};

} //namespace rt

#endif //_Triangle_HPP_INCLUDE_ONCE
