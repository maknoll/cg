#ifndef PARTICLEEMITTER_HPP_INCLUDE_ONCE
#define PARTICLEEMITTER_HPP_INCLUDE_ONCE

#include "OpenGL.hpp"
#include <vector>
#include <list>
#include <unordered_map>
#include <random>
#include "Particle.hpp"

namespace ogl
{
class CollisionScene;
class ParticleEmitter
{
public:

  struct Configuration
  {
    Configuration() : maxStepIterations(3), maxNumParticles(50000), 
      radius(0.1f), spawnrate(1000),
      gravity(0,0,-9.81f), lifeSpan(8,12),
      position(0,0,0), mainDirection(0,0,1),
      randomDirectionExponent(10000),
      velocitySpan(10,12),
      reflectDeviationExponent(100000),
      reflectVelocityDampeningSpan(0.5f,0.7f),
      forceParticleLifeSpan(4,4),
      enableFireworks(false)
    {}
    int maxStepIterations;
    int maxNumParticles;
    float radius;
    float spawnrate;
    Vec3 gravity;
    Vec2 lifeSpan;
    Vec3 position;
    Vec3 mainDirection;
    float randomDirectionExponent;
    Vec2 velocitySpan;
    float reflectDeviationExponent;
    Vec2 reflectVelocityDampeningSpan;
    Vec2 forceParticleLifeSpan;
    bool enableFireworks;
  };

  // The particle constructor.
  ParticleEmitter();

  // Free the memory
  virtual ~ParticleEmitter();

  // Initializes the emitter based on the current configuration
  bool init();

  // Performs an update step (kill, spawn and advect particles
  void step();

  // Creates a particle based on the current configuration
  // Note: the particle is not added!
  Particle createParticle() const;

  // Adds a particle to the system if free memory is available
  // The particle index is stored in particleIndex
  // The return value is true if the particle system has been added,
  // and false if not.
  bool addParticle(const Particle &p, size_t &particleIndex);

  // Adds a force particle to the particle set, the corresponding tracked 
  // 'normal particle' is created internally
  // Returns true if force particle could be added (free memory available),
  // and false if not.
  bool addForceParticle(std::shared_ptr<ForceParticle> forceParticle);

  // Copies rendering-related configuration to GPU
  void updateUniforms();

  // Binds the uniform buffer
  void bind(const GLuint shaderProgram,const GLuint bindingPoint, const std::string &blockName) const;

  // Set the collision scene to interact with
  void setCollisionScene(std::shared_ptr<CollisionScene> collisionScene)
  {
    mCollisionScene=collisionScene;
  }

  // Returns the maximum number of particles
  int numParticles() const { return (int)mParticles.size();}

  // Returns the VAO handle for the particle structs
  GLuint handle() const {return mVertexArrayObject;}

  Configuration& configuration() {return mConfig;}

private:

  // Copy particle buffer data to the GPU
  void updateParticlesOnGPU();

  // Advect a particle over time span dt
  void advectParticle(size_t particleIndex, const float dt);

  // Kill particles that will not last longer than time span dt
  void inactivateDyingParticles(float dt);

  // Spawn new particles over time span dt based on current configuration
  void spawnParticles(float dt);

  //Advect all particles over time span dt
  void advectActiveParticles(float dt);

  // Computes the sum of all forces including gravity and the forces,
  // of all other(!) force particles upon the particle with particleIndex.
  Vec3 sumParticleForces(size_t particleIndex);

  //Fireworks step
  void fireworkStep(float dt);

  // Frees CPU and GPU memory
  void clear();

  bool mInitialized;              //< True if init has been called after last clear() call.

  GLuint mVertexArrayObject;      //< Handle to the VAO
  GLuint mParticleBuffer;         //< Handle to the VBO holding the particle structs
  GLuint mUniformBuffer;          //< Handle to the UBO for geometry and Phong properties

  Configuration mConfig;          //< The current configuration struct

  Vec3 mBasisU, mBasisV, mBasisW; //< Cached orthonormal system for velocity direction of spawned particles

  float mGlobalTime;                //< The process runtime in milliseconds

  std::shared_ptr<CollisionScene> mCollisionScene; //Pointer to the collision scene

  std::vector<Particle> mParticles;     //< The particle data
  std::list<size_t> mActiveParticles;   //< List of indices of active particles
  std::list<size_t> mInactiveParticles; //< List of indices of inactive particles

  typedef std::unordered_map<size_t,std::shared_ptr<ForceParticle>> ForceParticleMap;
  ForceParticleMap mForceParticles; //<The hash map for forces attached to particles
};
} //namespace ogl

#endif //PARTICLEEMITTER_HPP_INCLUDE_ONCE
