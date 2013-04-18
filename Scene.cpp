#include "Scene.hpp"
#include "Renderable.hpp"
#include "Camera.hpp"
#include "Image.hpp"
#include <algorithm>

namespace rt
{

Scene::Scene() : mBackgroundColor(0,0,0,0)
{
}

Scene::~Scene()
{
}

std::shared_ptr<RayIntersection>
Scene::closestIntersection(const Ray &ray, real maxLambda)
{ 
  real closestLambda = maxLambda;
  std::shared_ptr<RayIntersection> tmpIntersection;
  std::shared_ptr<RayIntersection> closestIntersection;
  for (size_t i=0;i<mRenderables.size();++i)
  {
    std::shared_ptr<Renderable> r = mRenderables[i];
    if((tmpIntersection = r->closestIntersection(ray,closestLambda)))
    {
      if(tmpIntersection->lambda() < closestLambda)
      {
        closestLambda = tmpIntersection->lambda();
        closestIntersection = tmpIntersection;
      }
    }
  }
  return closestIntersection;
}

bool Scene::anyIntersection(const Ray &ray, real maxLambda)
{
  for (size_t i=0;i<mRenderables.size();++i)
  {
    std::shared_ptr<Renderable> r = mRenderables[i];
    if(r->anyIntersection(ray,maxLambda))
      return true;
  }
  return false;
}

}
