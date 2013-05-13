#include "Raytracer.hpp"
#include "Scene.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "Image.hpp"

namespace rt
{

Raytracer::Raytracer(size_t maxDepth) : mMaxDepth(maxDepth)
{
}

Raytracer::~Raytracer()
{
}

void Raytracer::renderToImage(std::shared_ptr<Image> image) const
{
  if(!mScene)
    return;

  if(!mScene->camera())
    return;

  Camera &camera = *(mScene->camera().get());
  camera.setResolution(image->width(),image->height());

  for(size_t y=0;y<image->height();++y)
    for(size_t x=0;x<image->width();++x)
    {
      // ray shot from camera position through camera pixel into scene
      const Ray ray = camera.ray(x,y);

      // call recursive raytracing function
      Vec4 color = this->trace(ray,0);
      image->setPixel(color,x,y);
    }
}

Vec4 Raytracer::trace(const Ray &ray, size_t depth) const
{
  std::shared_ptr<RayIntersection> intersection;
  if (intersection = mScene->closestIntersection(ray))
    return this->shade(intersection, depth);

  return mScene->backgroundColor();
}

Vec4 Raytracer::shade(std::shared_ptr<RayIntersection> intersection,
                      size_t depth) const
{
  // This offset must be added to intersection points for further
  // traced rays to avoid noise in the image
  const Vec3 offset(intersection->normal() * Math::safetyEps());

  Vec4 color(0,0,0,1);
  std::shared_ptr<const Renderable> renderable = intersection->renderable();
  std::shared_ptr<const Material>   material   = renderable->material();

  for(size_t i=0;i <mScene->lights().size();++i)
  {
    const Light &light = *(mScene->lights()[i].get());

    //Shadow ray from light to hit point.
    const Vec3 L = (intersection->position() + offset) - light.position();
    const Ray shadowRay(light.position(), L);

    //Shade only if light in visible from intersection point.
    if (!mScene->anyIntersection(shadowRay,L.norm()))
      color += material->shade(intersection,light);
  }
  return color;
}

} //namespace rt
