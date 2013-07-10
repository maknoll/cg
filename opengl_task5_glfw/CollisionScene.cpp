#include "CollisionScene.hpp"
#include "CollisionGeometry.hpp"

namespace ogl
{

std::shared_ptr<RayIntersection> CollisionScene::closestIntersection(const Ray &ray, float maxLambda) const
{
  float closestLambda = maxLambda;
  std::shared_ptr<RayIntersection> tmpIntersection;
  std::shared_ptr<RayIntersection> closestIntersection;
  for (size_t i=0;i<mGeometries.size();++i)
  {
    std::shared_ptr<CollisionGeometry> r = mGeometries[i];
    if(tmpIntersection = r->closestIntersection(ray,closestLambda))
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

} //namespace ogl