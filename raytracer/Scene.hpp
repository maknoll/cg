#ifndef SCENE_HPP_INCLUDE_ONCE
#define SCENE_HPP_INCLUDE_ONCE

#include <memory>
#include <vector>

#include "Math.hpp"

namespace rt
{
  class Image;
  class Camera;
  class Light;
  class Renderable;
  class Ray;
  class RayIntersection;

class Scene
{
public:
  Scene();
  virtual ~Scene();

  /// Add geometry to the scene.
  void addRenderable(std::shared_ptr<Renderable> renderable) {
    mRenderables.push_back(renderable);
  }

  /// Add a point light to the scene.
  void addLight(std::shared_ptr<Light> light) {
    mLights.push_back(light);
  }

  /// Returns a vector containing all lights in the scene.
  const std::vector<std::shared_ptr<Light>>& lights() const { return mLights; }

  /// Computes the closest intersection of a ray and any object in scene.
  std::shared_ptr<RayIntersection>
  closestIntersection(const Ray &ray,
                      real maxLambda = std::numeric_limits<real>::infinity());

  /// Checks whether a ray intersects any object in the scene.
  bool anyIntersection(const Ray &ray,
                       real maxLambda = std::numeric_limits<real>::infinity());

  const Vec4& backgroundColor() const { return mBackgroundColor; }
  std::shared_ptr<Camera> camera()    {return mCamera;}

  void setBackgroundColor(const Vec4& rgba)       { mBackgroundColor = rgba; }
  void setCamera(std::shared_ptr<Camera> camera) {mCamera=camera; }

private:
  Vec4 mBackgroundColor;

  std::shared_ptr<Camera> mCamera;

  std::vector<std::shared_ptr<Light>> mLights;
  std::vector<std::shared_ptr<Renderable>> mRenderables;
};

} //namespace rt

#endif //SCENE_HPP_INCLUDE_ONCE
