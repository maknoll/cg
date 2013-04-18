#ifndef CAMERA_HPP_INCLUDE_ONCE
#define CAMERA_HPP_INCLUDE_ONCE

#include "Ray.hpp"

namespace rt {

/// Abstract base class for camera.
class Camera
{
public:
  Camera();
  virtual ~Camera();

  /// Compute the primary ray passing through pixel x,y.
  virtual Ray ray(size_t x, size_t y) const = 0;

  // Constant accessors.
  const Vec3& position()     const { return mPosition; }
  const Vec3& lookAt()       const { return mLookAt; }
  const Vec3& up()           const { return mUp; }
  real horizontalFOV() const { return mHorizontalFOV; }
  real verticalFOV()   const { return mVerticalFOV; }
  size_t xResolution()       const { return mXResolution; }
  size_t yResolution()       const { return mYResolution; }

  // Mutator methods requiring reinitialization.

  void setPosition(const Vec3 &position)
  {
    mPosition=position;
    this->init();
  }

  /// Camera target point must not coincide with camera eye position.
  void setLookAt(const Vec3 &lookAt)
  {
    mLookAt=lookAt;
    this->init();
  }

  /// Camera target point must not coincide with direction from camera
  /// target to eye.
  void setUp(const Vec3 &up)
  {
    mUp=up;
    this->init();
  }

  void setFOV(real horizontalFOV, real verticalFOV)
  {
    mHorizontalFOV=horizontalFOV;
    mVerticalFOV=verticalFOV;
    this->init();
  }

  /// Set the camera image resolution. Valid values are > 0.
  void setResolution(size_t x, size_t y)
  {
    mXResolution=x;
    mYResolution=y;
    this->init();
  }

protected:

  // Use these accessors in derived classes.
  const Vec3& direction() const { return mDirection; }
  const Vec3& down()      const { return mDown; }
  const Vec3& right()     const { return mRight; }
  const Vec3& topLeft()   const { return mTopLeft; }

private:

  void init();

  Vec3  mPosition;            //!< World-space position of camera eye.
  Vec3  mLookAt;              //!< World-space position of camera target.
  Vec3  mUp;                  //!< World-space up vector.

  size_t mXResolution;        //!< Resolution in x-axis, i.e. width.
  size_t mYResolution;        //!< Resolution in y-axis, i.e. height.

  real  mHorizontalFOV;       //!< Horizontal field of view in degrees.
  real  mVerticalFOV;         //!< Vertical field of view in degrees.

  Vec3  mDirection;           //!< World-space vector from camera eye to target.
  Vec3  mDown;                //!< World-space down direction on camera plane.
  Vec3  mRight;               //!< World-space right direction on camera plane.
  Vec3  mTopLeft;             //!< World-space position of top
                              //!< left point on camera plane.
};

} //namespace rt

#endif //CAMERA_HPP_INCLUDE_ONCE
