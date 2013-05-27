#ifndef _TRIANGLEMESH_HPP_HPP_INCLUDE_ONCE
#define _TRIANGLEMESH_HPP_HPP_INCLUDE_ONCE

#include "Renderable.hpp"
#include "Triangle.hpp"

namespace rt
{

/// Stores a collection of triangles (or rather TriangleElements)
/// Supports the intersection test of a ray with any of the stored
/// triangles ( linear time w.r.t. the number of triangles).
class TriangleMesh : public Renderable
{
private:

  /// Lightweight storage container for triangle data:
  /// vertex position, vertex normal and uv parameters
  struct TriangleElement
  {
    /// Create a triangle with constant normals over the triangle and
    /// and no parametrization.
    TriangleElement(const Vec3 &_v0,const Vec3 &_v1,const Vec3 &_v2) :
      v0(_v0),v1(_v1),v2(_v2)
    {
      n0 = n1 = n2 = util::cross(v1-v0,v2-v0).normalized();
      uvw0 = uvw1 = uvw2 = Vec3(0,0,0);
    }

    /// Create a triangle with varying normals over the triangle and
    /// and no parametrization.
    TriangleElement(const Vec3 &_v0,const Vec3 &_v1,const Vec3 &_v2,
                    const Vec3 &_n0,const Vec3 &_n1,const Vec3 &_n2) :
    v0(_v0),v1(_v1),v2(_v2),n0(_n0),n1(_n1),n2(_n2)
    {
      uvw0 = uvw1 = uvw2 = Vec3(0,0,0);
    }

    /// Create a triangle with varying normals over the triangle and
    /// and a uv parametrization.
    TriangleElement(const Vec3 &_v0,const Vec3 &_v1,const Vec3 &_v2,
                    const Vec3 &_n0,const Vec3 &_n1,const Vec3 &_n2,
                    const Vec3 &_uvw0,const Vec3 &_uvw1,const Vec3 &_uvw2) :
      v0(_v0),v1(_v1),v2(_v2),
      n0(_n0),n1(_n1),n2(_n2),
      uvw0(_uvw0),uvw1(_uvw1),uvw2(_uvw2)
    {
      
    }
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 n0;
    Vec3 n1;
    Vec3 n2;
    Vec3 uvw0;
    Vec3 uvw1;
    Vec3 uvw2;
  };

public:
  
  /// Implements the intersection computation between ray and any stored triangle.
  std::shared_ptr<RayIntersection>
    closestIntersectionModel(const Ray &ray, real maxLambda) const override;

  bool anyIntersectionModel(const Ray &ray, real maxLambda) const override;

  void addTriangle(const Vec3 &v0,const Vec3 &v1,const Vec3 &v2)
  {
    mTriangles.push_back(TriangleElement(v0,v1,v2));
  }
  void addTriangle(const Vec3 &v0,const Vec3 &v1,const Vec3 &v2,
                   const Vec3 &n0,const Vec3 &n1,const Vec3 &n2)
  {
    mTriangles.push_back(TriangleElement(v0,v1,v2,n0,n1,n2));
  }
  void addTriangle(const Vec3 &v0,const Vec3 &v1,const Vec3 &v2,
                   const Vec3 &n0,const Vec3 &n1,const Vec3 &n2,
                   const Vec3 &uvw0,const Vec3 &uvw1,const Vec3 &uvw2)
  {
    mTriangles.push_back(TriangleElement(v0,v1,v2,n0,n1,n2,uvw0,uvw1,uvw2));
  }

protected:

  // Override this method to recompute the bounding box of this object.
  BoundingBox computeBoundingBox() const override;

private:
  std::vector<TriangleElement> mTriangles;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Intersection between plane and ray.
class TriangleMeshRayIntersection : public RayIntersection
{
public:
  TriangleMeshRayIntersection(const Ray &ray, const real lambda,
    std::shared_ptr<const Renderable> renderable,
    const Vec3 &normal, const Vec3 &uvw) :
  RayIntersection(ray,renderable,lambda,normal,uvw) {}
};

} //namespace rt

#endif //_TRIANGLEMESH_HPP_HPP_INCLUDE_ONCE