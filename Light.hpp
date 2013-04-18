#ifndef LIGHT_HPP_INCLUDE_ONCE
#define LIGHT_HPP_INCLUDE_ONCE

#include "Math.hpp"

namespace rt
{

/// Point light with position and spectral intensity.
class Light
{
public:
  Light(const Vec3 &position=Vec3(0,0,0),
        const Vec3 &spectralIntensity=Vec3(1,1,1));

  const Vec3& position()          const { return mPosition; }
  const Vec3& spectralIntensity() const { return mSpectralIntensity; }

  void setPosition(const Vec3 &position) { mPosition=position; }
  void setSpectralIntensity(const Vec3 &spectralIntensity)
  {
    mSpectralIntensity=spectralIntensity;
  }

private:
  Vec3 mPosition;
  Vec3 mSpectralIntensity;
};

} //namespace rt

#endif //LIGHT_HPP_INCLUDE_ONCE
