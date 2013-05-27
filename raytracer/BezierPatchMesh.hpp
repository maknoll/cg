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

protected:
    // Override this method to recompute the bounding box of this object.
    BoundingBox computeBoundingBox() const override;

private:


  std::pair<Vec3,Vec3> deCasteljau(const std::vector<Vec3> &curvePoints, real t) const;
  void deCasteljauRec(std::vector<Vec3> &points, real t) const;

  size_t mResU, mResV;              //!< triangle resolution in both parameter directions
  size_t mM, mN;                    //!< patch control point dimensions
  std::vector<Vec3> mControlPoints; //!< patch control points

};
} //namespace rt

#endif // _BEZIERPATCHMESH_HPP_HPP_INCLUDE_ONCE
