#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Math.hpp"
#include "Ray.hpp"

namespace rt
{

PhongMaterial::PhongMaterial(const Vec3& color, real reflectance, real shininess) :  
  Material(color,reflectance), mShininess(shininess)
{

}
Vec4 PhongMaterial::shade(std::shared_ptr<RayIntersection> intersection,
  const Light& light) const 
{
  // get normal and light direction
  Vec3 N = intersection->normal();
  Vec3 L = (light.position() - intersection->position()).normalized();

  real cosNL = std::max(N.dot(L),real(0));
  
  Vec3 V = intersection->ray().direction();
  Vec3 R = util::reflect(L, N);
  
  real cosRV = std::max(R.dot(V),real(0));

  // Programming TASK 3: Extend this method.
  // This method currently implements a Lambert's material with ideal
  // diffuse reflection.
  // Your task is to implement a Phong, or a Blinn-Phong shading model.
  Vec3 lightcolor = light.spectralIntensity() / 255;
  Vec3 diffuse = this->color()*cosNL;
  Vec3 specular = reflectance() * ((shininess() + 2) / (2 * M_PI)) * pow(cosRV,shininess()) * lightcolor;
  return Vec4(diffuse + specular, 1);
}

} //namespace rt
