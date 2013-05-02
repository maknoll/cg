#include "TriangleMesh.hpp"
#include "Intersection.hpp"

namespace rt
{


std::shared_ptr<RayIntersection>  TriangleMesh::closestIntersectionModel(const Ray &ray,
                                                                         real maxLambda) const
{
  real closestLambda = maxLambda;
  Vec3 closestbary;
  int  closestTri = -1;

  Vec3 bary;
  real lambda;

  // Loop over all stored triangles to find a possible intersection between ray 
  // and any triangle
  for (size_t i=0;i<mTriangles.size();++i)
  {
    const TriangleElement &tri = mTriangles[i];

    if (Intersection::lineTriangle(ray,tri.v0,tri.v1,tri.v2,bary,lambda) &&
      lambda > 0 && lambda < closestLambda)
    {
      closestLambda = lambda;
      closestbary = bary;
      closestTri = (int)i;
    }
  }

  // If an intersection occurred, compute the normal and the uv coordinates based
  // on the barycentric coordinates of the hit point
  if (closestTri >= 0)
  {
    const TriangleElement &tri = mTriangles[closestTri];
    Vec3 n = (tri.n0*closestbary[0]+tri.n1*closestbary[1]+
              tri.n2*closestbary[2]).normalized();
    Vec3 uvw = tri.uvw0*closestbary[0]+tri.uvw1*closestbary[1]+
               tri.uvw2*closestbary[2];
    return std::make_shared<TriangleMeshRayIntersection>(ray, closestLambda,
                                                         shared_from_this(),n,uvw);
  }

  return nullptr;
}

bool TriangleMesh::anyIntersectionModel(const Ray &ray, real maxLambda) const
{
  Vec3 uvw;
  real lambda;
  for (size_t i=0;i<mTriangles.size();++i)
  {
    const TriangleElement &tri = mTriangles[i];
    if (Intersection::lineTriangle(ray,tri.v0,tri.v1,tri.v2,uvw,lambda) &&
      lambda > 0 && lambda < maxLambda)
      return true;
  }
  return false;
}

} //namespace rt