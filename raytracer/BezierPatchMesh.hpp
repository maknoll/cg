#ifndef _BEZIERPATCHMESH_HPP_HPP_INCLUDE_ONCE
#define _BEZIERPATCHMESH_HPP_HPP_INCLUDE_ONCE

#include "TriangleMesh.hpp"

namespace rt {

// Bezier surface stores the set of control points and allows
// creation of a triangle mesh by sampling the parametric surface
class BezierPatchMesh : public TriangleMesh
{
public: 
  /**
   * @brief BezierMeshPatch constructor
   * @param m    number of control points in u direction
   * @param n    number of control points in v direction
   * @param resu triangle resolution in u direction
   * @param resv triangle resolution in v direction
   */
  BezierPatchMesh(size_t m,    size_t n,
                  size_t resu, size_t resv);

  // Must be called before rendering and after control point manipulation
  // Creates the set of triangles.
  void initialize();

  void setControlPoint(size_t i, size_t j, const Vec3& p)
  {
    mControlPoints[mM*j + i] = p;
  }
  const Vec3& controlPoint(size_t i, size_t j) const
  {
    return mControlPoints[mM*j + i];
  }

  struct BezierPatchSample
  {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
  };
  
  /** Samples the Bezier surface at the given point.
   * @brief sample
   * @param u coordinate of domain point
   * @param v coordinate of domain point
   * @return BezierPatchSample sample
   */
  BezierPatchSample sample(real u, real v) const;

private:

  size_t mResU, mResV;              //!< triangle resolution in both parameter directions
  size_t mM, mN;                    //!< patch control point dimensions
  std::vector<Vec3> mControlPoints; //!< patch control points
  Vec3 kabeljau(unsigned long, unsigned long, real, std::vector<Vec3>, std::function<int (unsigned long)>) const;
  Vec3 tangent(real, std::vector<Vec3>, unsigned long) const;
};
} //namespace rt

#endif // _BEZIERPATCHMESH_HPP_HPP_INCLUDE_ONCE
