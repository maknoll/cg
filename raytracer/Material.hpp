#ifndef MATERIAL_HPP_INCLUDE_ONCE
#define MATERIAL_HPP_INCLUDE_ONCE

#include "Math.hpp"
#include <memory>

namespace rt
{
class RayIntersection;
class Light;

/// Abstract material with shade and BRDF functions.
class Material
{
public:
  Material(const Vec3 &color = Vec3(0.5,0.5,0.5), real reflectance=0.0) :
    mColor(color), mReflectance(reflectance) {}

  virtual ~Material() {}

  // Returns the RGBA color for a point viewed from a direction 
  // lit by a light.
  virtual Vec4 shade(std::shared_ptr<RayIntersection> intersection, 
                     const Light& light) const = 0;

  const Vec3& color() const {return mColor;}

  real reflectance() const { return mReflectance; }

  /// Valid RGB color components have range [0,1].
	void setColor(const Vec3& color) { mColor=color; }

private:
Vec3 mColor;
real mReflectance;
};

}

#endif //MATERIAL_HPP_INCLUDE_ONCE
