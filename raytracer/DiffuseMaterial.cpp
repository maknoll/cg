#include "DiffuseMaterial.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "Ray.hpp"

namespace rt {

DiffuseMaterial::DiffuseMaterial(const Vec3& color) :  
  Material(color)
{

}

Vec4 DiffuseMaterial::shade(std::shared_ptr<RayIntersection> intersection,
                             const Light& light) const 
{
  Vec3 N = intersection->normal();
  Vec3 L = (light.position() - intersection->position()).normalized();

  real cosNL = std::max(N.dot(L),real(0));

  return Vec4(this->color()*cosNL,1.0);
}

} //namespace rt
