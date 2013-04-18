#ifndef CONSTANTMATERIAL_HPP_INCLUDE_ONCE
#define CONSTANTMATERIAL_HPP_INCLUDE_ONCE

#include "Material.hpp"

namespace rt
{

class RayIntersection;
class Light;

/// Simple material that has no shading.
class ConstantMaterial : public Material
{
public:
  ConstantMaterial(const Vec3& color = Vec3(0,0.4,0.8));

  Vec4 shade(std::shared_ptr<RayIntersection> intersection, 
             const Light& light) const override;
};

} //namespace rt

#endif // CONSTANTMATERIAL_HPP_INCLUDE_ONCE
