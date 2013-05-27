#include "BVHIndexedTriangleMesh.hpp"
#include "BVTree.hpp"
#include "Intersection.hpp"

namespace rt
{

BVHIndexedTriangleMesh::BVHIndexedTriangleMesh() : IndexedTriangleMesh()
{

}

void BVHIndexedTriangleMesh::initialize()
{
  mTree.build(this->vertexPositions(),*((const std::vector<Vec3i>*)(&this->triangleIndices())));
}

std::shared_ptr<RayIntersection>
  BVHIndexedTriangleMesh::closestIntersectionModel(const Ray &ray, real maxLambda) const
{
  const std::vector<int> &intersectionCandidates = mTree.intersectBoundingBoxes(ray,maxLambda);

  real closestLambda = maxLambda;
  Vec3 closestbary;
  int  closestTri = -1;

  Vec3 bary;
  real lambda;

  for(size_t i=0;i<intersectionCandidates.size();++i)
  {
    const int triangleIndex = intersectionCandidates[i];
    const int idx0 = this->triangleIndices()[3*triangleIndex+0];
    const int idx1 = this->triangleIndices()[3*triangleIndex+1];
    const int idx2 = this->triangleIndices()[3*triangleIndex+2];

    const Vec3 &p0 = this->vertexPositions()[idx0];
    const Vec3 &p1 = this->vertexPositions()[idx1];
    const Vec3 &p2 = this->vertexPositions()[idx2];

    if (Intersection::lineTriangle(ray,p0,p1,p2,bary,lambda) &&
      lambda > 0 && lambda < closestLambda)
    {
      closestLambda = lambda;
      closestbary = bary;
      closestTri = triangleIndex;
    }
  }

  if (closestTri >= 0)
  {
    const int i0 = this->triangleIndices()[3*closestTri+0];
    const int i1 = this->triangleIndices()[3*closestTri+1];
    const int i2 = this->triangleIndices()[3*closestTri+2];

    const Vec3 &p0 = this->vertexPositions()[i0];
    const Vec3 &p1 = this->vertexPositions()[i1];
    const Vec3 &p2 = this->vertexPositions()[i2];

    Vec3 n;
    if(this->vertexNormals().empty())
      n = util::cross(p1-p0,p2-p0).normalized();
    else
      n = this->vertexNormals()[i0]*closestbary[0]+
          this->vertexNormals()[i1]*closestbary[1]+
          this->vertexNormals()[i2]*closestbary[2];

    Vec3 uvw(0,0,0);
    if(!this->vertexTextureCoordinates().empty())
      uvw = this->vertexTextureCoordinates()[i0]*closestbary[0]+
            this->vertexTextureCoordinates()[i1]*closestbary[1]+
            this->vertexTextureCoordinates()[i2]*closestbary[2];

    return std::make_shared<IndexedTriangleMeshRayIntersection>(ray, closestLambda,
      shared_from_this(),n,uvw);
  }

  return nullptr;
 
}

bool BVHIndexedTriangleMesh::anyIntersectionModel(const Ray &ray, real maxLambda) const
{
  std::vector<int> intersectionCandidates = mTree.intersectBoundingBoxes(ray,maxLambda);


  Vec3 bary;
  real lambda;
  for(size_t i=0;i<intersectionCandidates.size();++i)
  {
    const int triangleIndex = intersectionCandidates[i];
    const int idx0 = this->triangleIndices()[3*triangleIndex+0];
    const int idx1 = this->triangleIndices()[3*triangleIndex+1];
    const int idx2 = this->triangleIndices()[3*triangleIndex+2];

    const Vec3 &p0 = this->vertexPositions()[idx0];
    const Vec3 &p1 = this->vertexPositions()[idx1];
    const Vec3 &p2 = this->vertexPositions()[idx2];

    if (Intersection::lineTriangle(ray,p0,p1,p2,bary,lambda) &&
      lambda > 0 && lambda < maxLambda)
      return true;
  }

  return false;
}
} //namespace rt