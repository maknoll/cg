#ifndef _INDEXEDTRIANGLEMESH_HPP_INCLUDE_ONCE
#define _INDEXEDTRIANGLEMESH_HPP_INCLUDE_ONCE

#include "Renderable.hpp"
#include "Ray.hpp"

namespace rt
{

class IndexedTriangleMesh : public Renderable
{
public:

  /// Implements the intersection computation between ray and any stored triangle.
  std::shared_ptr<RayIntersection>
    closestIntersectionModel(const Ray &ray, real maxLambda) const override;

  bool anyIntersectionModel(const Ray &ray, real maxLambda) const override;

  bool loadFromOBJ(const std::string &filePath);
  bool saveToOBJ(const std::string &filePath, bool textureCoordinates=true, bool normals=true) const;


  const std::vector<Vec3>& vertexPositions()          const {return mVertexPosition;}
  const std::vector<Vec3>& vertexTextureCoordinates() const {return mVertexTextureCoordinate;}
  const std::vector<Vec3>& vertexNormals()            const {return mVertexNormal;}
  const std::vector<int>&  triangleIndices()          const {return mIndices;}

protected:

  // Override this method to recompute the bounding box of this object.
  BoundingBox computeBoundingBox() const override;

private:
  std::vector<Vec3>                   mVertexPosition;
  std::vector<Vec3>                   mVertexTextureCoordinate;
  std::vector<Vec3>                   mVertexNormal;
  std::vector<int>                    mIndices;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class IndexedTriangleMeshRayIntersection : public RayIntersection
{
public:
  IndexedTriangleMeshRayIntersection(const Ray &ray, const real lambda,
    std::shared_ptr<const Renderable> renderable,
    const Vec3 &normal, const Vec3 &uvw) :
  RayIntersection(ray,renderable,lambda,normal,uvw) {}
};

} //rt


#endif //_INDEXEDTRIANGLEMESH_HPP_INCLUDE_ONCE