#ifndef RAYTRACER_HPP_INCLUDE_ONCE
#define RAYTRACER_HPP_INCLUDE_ONCE

#include <vector>
#include <string>
#include <memory>

#include "Math.hpp"

namespace rt
{

class Scene;
class Ray;
class RayIntersection;
class Image;

/// Performs recursive raytracing.
class Raytracer
{
public:
  Raytracer(size_t maxDepth=10);
  virtual ~Raytracer();

  /// The scene contains all Renderables, Lights, and a Camera.
  void setScene(std::shared_ptr<Scene> scene) { mScene=scene; }

  /// Writes RGBA values to an image.
  void renderToImage(std::shared_ptr<Image> image) const;

protected:

  /// Returns the color of a traced ray.
  Vec4 trace(const Ray &ray,
             size_t depth) const;

  /// Determines the color of an intersection point.
  Vec4 shade(std::shared_ptr<RayIntersection> intersection,
             size_t depth) const;

private:
  size_t mMaxDepth;              ///< Maximum number of ray indirections.
  std::shared_ptr<Scene> mScene;
};

} //namespace rt

#endif //RAYTRACER_HPP_INCLUDE_ONCE
