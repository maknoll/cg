#include "Renderable.hpp"
#include "Ray.hpp"
namespace rt
{

Renderable::Renderable() : mTransformClean(true)
{

}

Renderable::~Renderable()
{

}

bool Renderable::anyIntersectionModel(const Ray &ray, real maxLambda) const
{
  return this->closestIntersectionModel(ray,maxLambda) != nullptr;
}

std::shared_ptr<RayIntersection>
Renderable::closestIntersection(const Ray &ray, real maxLambda) const
{
  //adapt maximal lambda value relative to transformation properties
  //(e.g., scaling)
  maxLambda = transformRayLambdaWorldToModel(ray, maxLambda);

  //transform ray from world to model coordinate system
  std::shared_ptr<RayIntersection> isect_local =
      this->closestIntersectionModel(transformRayWorldToModel(ray),maxLambda);

  if (!isect_local)
    return nullptr;

  //transform intersection from model to world coordinate system
  isect_local->transform(mTransform, mTransformInvTransp);

  return isect_local;
}

bool Renderable::anyIntersection(const Ray &ray, real maxLambda) const
{
  maxLambda = transformRayLambdaWorldToModel(ray, maxLambda);
  return this->anyIntersectionModel(transformRayWorldToModel(ray),maxLambda);
}

void Renderable::updateTransforms() const
{
  if(!mTransformClean)
  {
    mTransformInv = mTransform;

    // The matrix should always be invertible. If this wasn't the case
    // an error should be thrown.
    if(!mTransformInv.invert())
    {
      std::cerr<<"Renderable::transformRayLocal: Error: tranformation not invertible"<<std::endl;
    }
    mTransformInvTransp = mTransformInv.transposed();
    mTransformClean = true;
  }
}

Ray Renderable::transformRayWorldToModel(const Ray &ray) const
{
  this->updateTransforms();
  return ray.transformed(mTransformInv);
}

real Renderable::transformRayLambdaWorldToModel(const Ray &ray, const real lambda) const
{
  this->updateTransforms();
  Vec3 model_direction = mTransformInv.transformVector(ray.direction());

  return lambda * model_direction.norm();
}

}
