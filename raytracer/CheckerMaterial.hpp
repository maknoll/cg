#ifndef _CHECKERMATERIAL_HPP_INCLUDE_ONCE
#define _CHECKERMATERIAL_HPP_INCLUDE_ONCE

#include "Material.hpp"

namespace rt
{

/// Material that creates a checkerboard based on the uv parametrization
/// at the intersection point. It calls the shade function of either 
/// material1 o material2
class CheckerMaterial : public Material
{
public:
  CheckerMaterial(std::shared_ptr<Material> material1,
                  std::shared_ptr<Material> material2,
                  const Vec2 &tiles = Vec2(1,1));

  Vec4 shade(std::shared_ptr<RayIntersection> intersection, 
    const Light& light) const override;

private:
  Vec2 mTiles; ///< number of tiles per uv in [0,1]x[0,1]
  std::shared_ptr<Material> mMaterial1;
  std::shared_ptr<Material> mMaterial2;
};
} //namespace rt

#endif //_CHECKERMATERIAL_HPP_INCLUDE_ONCE