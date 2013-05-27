#ifndef PHONGMATERIAL_HPP_INCLUDE_ONCE
#define PHONGMATERIAL_HPP_INCLUDE_ONCE

#include "Material.hpp"

namespace rt
{

  class RayIntersection;
  class Light;

class PhongMaterial : public Material
{
public:
  PhongMaterial(const Vec3& color = Vec3(0,0.4,0.8),
                real reflectance=1.0,
                real shininess=10.0);

  Vec4 shade(std::shared_ptr<RayIntersection> intersection, 
    const Light& light) const override;

  real shininess() const { return mShininess; }

private:

  real mShininess; 
};

} //namespace rt

#endif // PHONGMATERIAL_HPP_INCLUDE_ONCE
