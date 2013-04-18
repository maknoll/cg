#include "Camera.hpp"
#include "Math.hpp"

namespace rt {

Camera::Camera() : mPosition(1,1,1), mLookAt(0,0,0), mUp(0,0,1),
                   mXResolution(1), mYResolution(1),
                   mHorizontalFOV(90), mVerticalFOV(90)
{
  this->init();
}

Camera::~Camera()
{

}

void Camera::init()
{
  mDirection = (mLookAt - mPosition).normalize();

  mRight = (cross(mDirection, mUp))   .normalize();
  mDown  = (cross(mDirection, mRight)).normalize();

  mRight = mRight * tan(mHorizontalFOV * M_PI / 360);
  mDown  = mDown *  tan(mVerticalFOV   * M_PI / 360);

  mTopLeft = mPosition + mDirection - mRight + mDown;

  mRight = 2.0 * mRight / real(mXResolution);
  mDown  = 2.0 * mDown  / real(mYResolution);
}

} //namespace rt
