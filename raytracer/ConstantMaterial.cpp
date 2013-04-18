#include "ConstantMaterial.hpp"

namespace rt {

ConstantMaterial::ConstantMaterial(const Vec3& color) :  
  Material(color)
{

}

Vec4 ConstantMaterial::shade(std::shared_ptr<RayIntersection> intersection,
                             const Light& light) const 
{
   return Vec4(this->color(),1.0);
}

} //namespace rt
