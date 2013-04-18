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
  Triangle( const Vec3 &vertex0, const Vec3 &vertex1, const Vec3 &vertex2);

  const Vec3* positions() const { return mVertices; }

protected:
  /// Implements the intersection computation between ray and Triangle.
  std::shared_ptr<RayIntersection>
  closestIntersectionModel(const Ray &ray, real maxLambda) const override;

private:

  // Vertex positions
  Vec3 mVertices[3];
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Intersection between Triangle and ray.
class TriangleRayIntersection : public RayIntersection
{
public:
  TriangleRayIntersection(const Ray &ray, const real lambda,
                          std::shared_ptr<const Renderable> renderable,
                          const Vec3 &normal) :
    RayIntersection(ray,renderable,lambda,normal) {}
};

} //namespace rt

#endif //_Triangle_HPP_INCLUDE_ONCE
