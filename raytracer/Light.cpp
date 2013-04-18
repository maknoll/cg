#include "Light.hpp"

namespace rt
{

Light::Light(const Vec3 &position, const Vec3 &spectralIntensity) :
  mPosition(position), mSpectralIntensity(spectralIntensity)
{

}

} //namespace rt
