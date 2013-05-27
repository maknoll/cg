#ifndef BVHINDEXEDTRIANGLEMESH_HPP_INCLUDE_ONCE
#define BVHINDEXEDTRIANGLEMESH_HPP_INCLUDE_ONCE

#include "IndexedTriangleMesh.hpp"
#include "BVTree.hpp"

namespace rt
{
class BVHIndexedTriangleMesh : public IndexedTriangleMesh
{
public:
  BVHIndexedTriangleMesh();

  void initialize() override;

  std::shared_ptr<RayIntersection>
    closestIntersectionModel(const Ray &ray, real maxLambda) const override;

  bool anyIntersectionModel(const Ray &ray, real maxLambda) const override;

private:
  BVTree mTree;
};
} //namespace rt

#endif //BVHINDEXEDTRIANGLEMESH_HPP_INCLUDE_ONCE