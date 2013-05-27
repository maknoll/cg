#include "IndexedTriangleMesh.hpp"
#include "IndexedTriangleIO.hpp"
#include "Intersection.hpp"

namespace rt
{


std::shared_ptr<RayIntersection>  IndexedTriangleMesh::closestIntersectionModel(const Ray &ray,
  real maxLambda) const
{

  real closestLambda = maxLambda;
  Vec3 closestbary;
  int  closestTri = -1;

  Vec3 bary;
  real lambda;

  // Loop over all stored triangles to find a possible intersection between ray 
  // and any triangle
  for (size_t i=0;i<mIndices.size();i+=3)
  {
    const int i0 = mIndices[i+0];
    const int i1 = mIndices[i+1];
    const int i2 = mIndices[i+2];
    if (Intersection::lineTriangle(ray,mVertexPosition[i0],mVertexPosition[i1],mVertexPosition[i2],bary,lambda) &&
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
    const int i0 = mIndices[closestTri+0];
    const int i1 = mIndices[closestTri+1];
    const int i2 = mIndices[closestTri+2];

    Vec3 n;
    if(mVertexNormal.empty())
      n = util::cross(mVertexPosition[i1]-mVertexPosition[i0],mVertexPosition[i2]-mVertexPosition[i0]).normalized();
    else
      n = (mVertexNormal[i0]*closestbary[0]+
           mVertexNormal[i1]*closestbary[1]+
           mVertexNormal[i2]*closestbary[2]).normalized();

    Vec3 uvw(0,0,0);
    if(!mVertexTextureCoordinate.empty())
      uvw = (mVertexTextureCoordinate[i0]*closestbary[0]+
             mVertexTextureCoordinate[i1]*closestbary[1]+
             mVertexTextureCoordinate[i2]*closestbary[2]);
    return std::make_shared<IndexedTriangleMeshRayIntersection>(ray, closestLambda,
      shared_from_this(),n,uvw);
  }

  return nullptr;
}

bool IndexedTriangleMesh::anyIntersectionModel(const Ray &ray, real maxLambda) const
{
  Vec3 uvw;
  real lambda;
  for (size_t i=0;i<mIndices.size();i+=3)
  {
    const int i0 = mIndices[i+0];
    const int i1 = mIndices[i+1];
    const int i2 = mIndices[i+2];
    if (Intersection::lineTriangle(ray,mVertexPosition[i0],mVertexPosition[i1],mVertexPosition[i2],uvw,lambda) &&
      lambda > 0 && lambda < maxLambda)
      return true;
  }
  return false;
}

bool IndexedTriangleMesh::loadFromOBJ(const std::string &filePath)
{
  IndexedTriangleIO io;
  if(!io.loadFromOBJ(filePath))
    return false;

  //loading succeeded, copy data
  mVertexPosition=std::vector<Vec3>(io.vertexPositions());
  mVertexTextureCoordinate=std::vector<Vec3>(io.vertexTextureCoordinates());
  mVertexNormal=std::vector<Vec3>(io.vertexNormals());
  mIndices=std::vector<int>(io.triangleIndices());

  return true;
}

bool IndexedTriangleMesh::saveToOBJ(const std::string &filePath,
                                    bool textureCoordinates,
                                    bool normals) const
{
  IndexedTriangleIO io;

  //set io data
  io.setVertexPositions(mVertexPosition);
  io.setVertexTextureCoordinates(mVertexTextureCoordinate);
  io.setVertexNormals(mVertexNormal);
  io.setTriangleIndices(mIndices);
  return io.saveToOBJ(filePath,textureCoordinates,normals);
}

BoundingBox IndexedTriangleMesh::computeBoundingBox() const
{
  BoundingBox bbox;
  for (size_t i=0;i<mVertexPosition.size();++i)
    bbox.expandByPoint(mVertexPosition[i]);
  return bbox;
}

} //rt