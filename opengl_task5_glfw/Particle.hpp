#ifndef FORCEPARTICLE_HPP_INCLUDE_ONCE
#define FORCEPARTICLE_HPP_INCLUDE_ONCE

#include "OpenGL.hpp"

namespace ogl
{
  //The particle struct is copied to the GPU for rendering
  struct Particle
  {
    Particle() : lifeLeft(0.f), isSpecialParticle(0.f) {}
    Vec3 position;            //<Particle world position.
    Vec3 velocity;            //<Particle velocity vector.
    float lifeLeft;           //<Particle life left in seconds (dies when lifeLeft ==0)
    float isSpecialParticle;  //<Particle status, 0->normal, 1 ->force particle
  };

//Abstract class for particles that exert forces to other particles
class ForceParticle
{
public:

  // This function must be implemented by force particles.
  // The first parameter is the world position of the force particle.
  // The second parameter is the world position of the other particle (that the force acts upon)
  // The return value is a force vector in world coordinates.
  virtual Vec3 computeForce(const Particle &forceParticle, const Vec3 &otherParticlePosition) const=0;
};

class PointGravitySource : public ForceParticle
{
public:
  Vec3 computeForce(const Particle & forceParticle, const Vec3 &otherParticlePosition) const override
  {
    // TODO: 
    // Your task is to implement a force function as described in the exercise.
    // Experiment with different formulas and test the outcome.
    return Vec3(0,0,0);
  }
};

}

#endif //FORCEPARTICLE_HPP_INCLUDE_ONCE