#ifndef BVTREE_HPP_INCLUDE_ONCE
#define BVTREE_HPP_INCLUDE_ONCE

#include <vector>
#include <stack>
#include "Math.hpp"
#include "BoundingBox.hpp"

namespace rt
{

class BVTree
{
public:


  //build from indexed triangle set
  void build(const std::vector<Vec3> &vertexPositions,const std::vector<Vec3i> &triangleIndices);

  //returns a set of triangle indices as candidates for ray-triangle intersection
  const std::vector<int> intersectBoundingBoxes(const Ray &ray, const real maxLambda) const;
private:

  struct Node
  {
    Node() {left=0;right=0;}
    int left;
    int right;
    BoundingBox bbox;
  };
  void sortTriangles();
  void createNodes(const std::vector<Vec3> &vertexPositions,
    const std::vector<Vec3i> &triangleIndices);

  void printSortedIndicesStatus(size_t offset, size_t numTriangles);
  void buildHierarchy(size_t rootNodeIndex, size_t numTriangles,size_t offset);

  void computeBoundingBoxAreas(size_t offset, size_t numTriangles);

  std::vector<Node> mNodes;

  std::vector<bool>        mTempMarker;
  std::vector<BoundingBox> mTempTriangleBoxes;
  std::vector<Vec3i>       mTempSortedTriangleIndices;
  std::vector<Vec3i>       mTempBufferTriangleIndices;
  std::vector<Vec3>        mTempAreasLeft;
  std::vector<Vec3>        mTempAreasRight;

};
}

#endif //BVTREE_HPP_INCLUDE_ONCE