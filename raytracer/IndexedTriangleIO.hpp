#ifndef _INDEXEDTRIANGLEIO_INCLUDE_ONCE
#define _INDEXEDTRIANGLEIO_INCLUDE_ONCE

#include "Math.hpp"
#include <map>
#include <vector>

namespace rt
{

class IndexedTriangleIO
{
public:
  void clear();
  bool loadFromOBJ(const std::string &filePath);
  bool saveToOBJ(const std::string &filePath, bool textureCoordinates=true, bool normals=true) const;

  const std::vector<Vec3>& vertexPositions()          const {return mVertexPosition;}
  const std::vector<Vec3>& vertexTextureCoordinates() const {return mVertexTextureCoordinate;}
  const std::vector<Vec3>& vertexNormals()            const {return mVertexNormal;}
  const std::vector<int>&  triangleIndices()          const {return mIndices;}

  void setVertexPositions(const std::vector<Vec3>& v) {mVertexPosition=std::vector<Vec3>(v);}
  void setVertexTextureCoordinates(const std::vector<Vec3>& v) {mVertexTextureCoordinate=std::vector<Vec3>(v);}
  void setVertexNormals(const std::vector<Vec3>& v) {mVertexNormal=std::vector<Vec3>(v);}
  void setTriangleIndices(const std::vector<int>& v) {mIndices=std::vector<int>(v);}

private:

  // Container for printing warnings on Wavefront OBJ format specs
  // that are unsupported by this loader. 
  struct UnsupportedFeatures
  {
    UnsupportedFeatures() : hasLine(false),
      hasGroup(false),
      hasSmoothingGroup(false),
      hasMaterial(false) 
    { }

    bool hasLine;
    bool hasGroup;
    bool hasSmoothingGroup;
    bool hasMaterial;
  };
  
  // Vertex flattening or duplication needs strict weak ordering
  struct CompareVec3i
  {
    bool operator()(const Vec3i& lhs, const Vec3i& rhs) const
    {
      for (int i=0;i<3;++i)
      {
        if (lhs[i] < rhs[i]) return true;
        if (lhs[i] > rhs[i]) return false;
      }
      return false;
    }
  };

  void insertFlattenedTriangleV  (const Vec3i &v);
  void insertFlattenedTriangleVT (const Vec3i &v,const Vec3i &t);
  void insertFlattenedTriangleVN (const Vec3i &v,const Vec3i &n);
  void insertFlattenedTriangleVTN(const Vec3i &v,const Vec3i &t,const Vec3i &n);
  int insertFlattenedVertexVTN(const int v, const int t, const  int n);
  void clearCaches();

  std::vector<Vec3>                   mVertexPosition;
  std::vector<Vec3>                   mVertexTextureCoordinate;
  std::vector<Vec3>                   mVertexNormal;
  std::vector<int>                    mIndices;

  std::vector<Vec3>                   mVertexPositionCache;
  std::vector<Vec3>                   mVertexTextureCoordinateCache;
  std::vector<Vec3>                   mVertexNormalCache;
  std::map<Vec3i,size_t,CompareVec3i> mIndicesCache;
};

} //namespace rt


#endif //_INDEXEDTRIANGLEIO_INCLUDE_ONCE