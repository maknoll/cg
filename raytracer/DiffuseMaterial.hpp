#ifndef DIFFUSEMATERIAL_HPP_INCLUDE_ONCE
#define DIFFUSEMATERIAL_HPP_INCLUDE_ONCE

#include "Material.hpp"

namespace rt
{

class RayIntersection;
class Light;

/// Material based on Lambert's cosine law
/// see http://en.wikipedia.org/wiki/Lambert%27s_cosine_law
class DiffuseMaterial : public Material
{
public:
  DiffuseMaterial(const Vec3& color = Vec3(0,0.4,0.8));

  Vec4 shade(std::shared_ptr<RayIntersection> intersection, 
             const Light& light) const override;
};

} //namespace rt

#endif // DIFFUSEMATERIAL_HPP_INCLUDE_ONCE
