#ifndef RAY_HPP_INCLUDE_ONCE
#define RAY_HPP_INCLUDE_ONCE

#include <memory>

#include "Math.hpp"

namespace rt
{

class Renderable;

/// Ray consists of point and direction
class Ray
{
public:
  Ray() : mOrigin(0.0,0.0,0.0), mDirection(0.0,0.0,0.0) {}
  Ray(const Vec3 &origin, const Vec3 &direction) :
    mOrigin(origin), mDirection(direction.normalized())  {}

  Vec3 pointOnRay(real lambda) const { return mOrigin + mDirection*lambda; }

  const Vec3& origin()    const { return mOrigin; }
  const Vec3& direction() const { return mDirection; }

  void setOrigin(const Vec3& origin)       { mOrigin=origin; }
  void setDirection(const Vec3& direction) { mDirection=direction.normalized(); }

  void transform(const Mat4 &transform)
  {
    mOrigin    = transform.transformPoint(mOrigin);
    mDirection = transform.transformVector(mDirection).normalized();
  }

  Ray transformed(const Mat4 &transform) const
  {
    return Ray(transform.transformPoint(mOrigin),
               transform.transformVector(mDirection));
  }

private:
  Vec3 mOrigin;
  Vec3 mDirection;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Container class for intersection between a renderable and a ray
class RayIntersection
{
public:

  /// All necessary information for intersection must be passed to
  /// constructor. No mutator methods are available.
  RayIntersection(const Ray &ray,
                  std::shared_ptr<const Renderable> renderable,
                  const real lambda, const Vec3 &normal) :
    mRay(ray), mRenderable(renderable), mLambda(lambda), mNormal(normal)
  {
    mPosition=ray.pointOnRay(mLambda);
  }

  virtual ~RayIntersection() {}

  const Ray& ray()                               const { return mRay; }
  std::shared_ptr<const Renderable> renderable() const { return mRenderable; }
  real lambda()                                  const { return mLambda; }
  const Vec3& position()                         const { return mPosition; }
  const Vec3& normal()                           const { return mNormal; }

  virtual void transform(const Mat4 &transform,
                         const Mat4 &transformInvTransp)
  {

    //transform local intersection information to global system
    mPosition = transform.transformPoint(mPosition);
    mNormal   = transformInvTransp.transformVector(mNormal).normalize();
    mRay.transform(transform);
    mLambda   = (mPosition - mRay.origin()).norm();
  }

protected:
  Ray mRay;
  std::shared_ptr<const Renderable> mRenderable;
  real mLambda;
  Vec3 mPosition;
  Vec3 mNormal;
};

} //namespace rt

#endif //RAY_HPP_INCLUDE_ONCE
