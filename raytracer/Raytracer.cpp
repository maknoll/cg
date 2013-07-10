#include "Raytracer.hpp"
#include "Scene.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "Image.hpp"
#include <thread>

#define THREADS 16

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

  mScene->prepareScene();

  Camera &camera = *(mScene->camera().get());
  camera.setResolution(image->width(),image->height());

  std::thread threads[THREADS];
  
  for (int i = 0; i < THREADS; i++) {
	threads[i] = std::thread([&](int i) {
	  for(size_t y = (image->height() / THREADS) * i; y < (image->height() / THREADS) * (i + 1); ++y)
		for(size_t x = 0;x < image->width(); ++x)
		{
		  // ray shot from camera position through camera pixel into scene
		  const Ray ray = camera.ray(x,y);
		  
		  // call recursive raytracing function
		  Vec4 color = this->trace(ray,0);
		  image->setPixel(color,x,y);
		}
	}, i);
  }
  
  for (int i = 0; i < THREADS; i++)
	threads[i].join();
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
  std::shared_ptr<const Image>    texture    = renderable->texture();

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

  if (depth<mMaxDepth)
  {
    const Vec3 &N(intersection->normal());
	
    // get incident viewing vector
    const Vec3 &I = intersection->ray().direction();
	
    real t= material->reflectance();
	
    if (t>real(0))
    {
      // get out-going viewing direction (reflect)
      Vec3 D = reflect(I, N).normalized();
	  
      // calculate incident radiance by recursive ray tracing
      const Ray  r(intersection->position()+offset, D);
      const Vec4 incident_radiance = this->trace(r,++depth);
	  
      // how much of the incident radiance is reflected toward the viewer?
      color = color*(1.0-t) + incident_radiance * Vec4(material->color(),1) * t;
      color[3]=1.0;
	}
  }

  return color;
}

} //namespace rt
