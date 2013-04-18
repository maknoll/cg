#ifndef RENDERABLE_HPP_INCLUDE_ONCE
#define RENDERABLE_HPP_INCLUDE_ONCE

#include <memory>
#include <vector>

#include "Math.hpp"

namespace rt
{
class Material;
class Ray;
class RayIntersection;

  /// Abstract class for visible geometry.
  class Renderable : public std::enable_shared_from_this<Renderable>
  {
  public:
    // Constructor.
    Renderable();
    // Destructor.
    virtual ~Renderable();

    // Computes the point of intersection between ray and object.
    std::shared_ptr<RayIntersection>
      closestIntersection(const Ray &ray, real maxLambda) const;

    // This is used for so-called 'any hit' rays (returns true if there is at
    // least one intersection.)
    bool anyIntersection(const Ray &ray, real maxLambda) const;

    // Gets the transformation.
    Mat4& transform()
    {
      mTransformClean = false;
      return mTransform;
    }

    // Gets the material.
    std::shared_ptr<const Material> material() const { return mMaterial; }
    // Sets the material.
    void setMaterial(std::shared_ptr<Material> material) { mMaterial = material; }

  protected:

    // This function does the ray intersection test in the local model coordinate
    // system of the object. Override this function for each class inheriting
    // from Renderable.
    virtual std::shared_ptr<RayIntersection>
      closestIntersectionModel(const Ray &ray, real maxLambda) const = 0;

    // This function does an any hit ray intersection test in the local model
    // coordinate system of the object. By default, closestIntersectionLocal
    // is called. For most geometric primitives, there are faster methods to
    // test if there is at least one intersection rather than computing the
    // closest. If so, override this method and perform the faster test.
    virtual bool anyIntersectionModel(const Ray &ray, real maxLambda) const;

  private:
  Mat4 mTransform;
  std::shared_ptr<Material> mMaterial;

  mutable bool mTransformClean;
  mutable Mat4 mTransformInv, mTransformInvTransp;
  void updateTransforms() const;
  Ray  transformRayWorldToModel(const Ray &ray) const;
  real transformRayLambdaWorldToModel(const Ray &ray, const real lambda) const;
};

} //namespace rt

#endif //RENDERABLE_HPP_INCLUDE_ONCE
