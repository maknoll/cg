#include "ParticleEmitter.hpp"
#include "Collision.hpp"
#include "CollisionScene.hpp"

namespace ogl
{

ParticleEmitter::ParticleEmitter() : mInitialized(false),
  mGlobalTime(0)
{
}

ParticleEmitter::~ParticleEmitter()
{
  this->clear();
}

bool ParticleEmitter::init()
{
  this->clear();

  mParticles.resize(mConfig.maxNumParticles);

  for(size_t i = 0; i<mParticles.size();++i)
  {
    mInactiveParticles.push_back(i);
  }
  // Create and copy particles to GPU
  glGenBuffers(1, &mParticleBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * mParticles.size(), 0, GL_STATIC_DRAW);
  ogl::printOpenGLError();

  // Generate a vertex array object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffer);
  size_t f = sizeof(float);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(3*f));
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(6*f));
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(7*f));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);

  //Create uniform buffer
  glGenBuffers(1, &mUniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 2, NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  
  mGlobalTime = float(glfwGetTime());

  Math::orthonormalBasis(mConfig.mainDirection,mBasisU,mBasisV,mBasisW);
  
  return true;
}

Particle ParticleEmitter::createParticle() const
{
  Particle p;

  //The lifespan is uniformly distributed.
  p.lifeLeft = Math::random(mConfig.lifeSpan[0],mConfig.lifeSpan[1]);
  p.position = mConfig.position;

  //The initial velocity direction is based on a Phong lobe around mConfig.mainDirection)
  Vec3 sampleDir = Math::samplePhongLobe(mConfig.randomDirectionExponent);
  p.velocity =(mBasisU * sampleDir[0] + mBasisV * sampleDir[1] + mBasisW * sampleDir[2]).normalize();

  //The initial velocity length is uniformly distributed
  p.velocity *=Math::random(mConfig.velocitySpan[0],mConfig.velocitySpan[1]);

  return p;
}

void ParticleEmitter::step()
{
  // Compute the time step in seconds
  float oldTime=mGlobalTime;
  mGlobalTime=float(glfwGetTime());
  float dt = (mGlobalTime-oldTime); //time step in seconds

  if(mConfig.enableFireworks)
    this->fireworkStep(dt);

  // Inactivate particles that will die within this step
  this->inactivateDyingParticles(dt);

  // Spawn new particles
  this->spawnParticles(dt);

  // Advect the active particles
  this->advectActiveParticles(dt);

  // Copy updated data to GPU
  updateParticlesOnGPU();
}


bool ParticleEmitter::addParticle(const Particle &p, size_t &particleIndex)
{
  // We have no more free memory for a new particle
  if(mInactiveParticles.empty())
    return false;

  // Get and remove the index from the inactive ones
  particleIndex = mInactiveParticles.front();
  mInactiveParticles.pop_front();

  // Set the new particle
  mParticles[particleIndex]=p;

  // Add to the list of active particles
  mActiveParticles.push_back(particleIndex);

  return true;
}

bool ParticleEmitter::addForceParticle(std::shared_ptr<ForceParticle> forceParticle)
{
  // Create a 'normal particle' which a force particle is attached to
  Particle p = this->createParticle();
  p.isSpecialParticle=1.f;

  // The force particle lifespan differs from the normal one
  p.lifeLeft = Math::random(mConfig.forceParticleLifeSpan[0],mConfig.forceParticleLifeSpan[1]);

  size_t particleIndex;
  if(!this->addParticle(p,particleIndex))
    return false; // no more free memory?

  // Add force particle to list
  mForceParticles[particleIndex]=forceParticle;

  return true;
}

void ParticleEmitter::updateUniforms()
{
  // Upload the new particle rendering properties to the GPU
  glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 0, sizeof(float) * 1,&mConfig.radius);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 1, sizeof(float) * 1,&mConfig.lifeSpan[1]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ParticleEmitter::bind(const GLuint shaderProgram,const GLuint bindingPoint, const std::string &blockName) const
{
  GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, blockName.c_str());
  glUniformBlockBinding(shaderProgram, uniformBlockIndex, bindingPoint);
  glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, mUniformBuffer, 0, sizeof(float)*1);
}

void ParticleEmitter::updateParticlesOnGPU()
{
  // Copy all particle data to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * mParticles.size(), &mParticles[0]);
  ogl::printOpenGLError();
}

void ParticleEmitter::clear()
{
  if(!mInitialized)
    return;

  mParticles.clear();
  mParticles.shrink_to_fit();
  mActiveParticles.clear();
  mInactiveParticles.clear();

  glDeleteVertexArrays(1,&mVertexArrayObject);
  glDeleteBuffers(1,&mParticleBuffer);
  glDeleteBuffers(1,&mUniformBuffer);

  mInitialized = false;
}
void ParticleEmitter::spawnParticles(float dt)
{
  // Compute the amount of new particles for this time step
  int n = int(dt*mConfig.spawnrate);

  // Compute the fraction of the new particle amount
  // This makes it possible to, e.g., emit 0.1 particles per second
  float nFrac = dt*mConfig.spawnrate - n;
  if(Math::random() < nFrac)
    ++n;

  for(size_t i= 0; i < n;++i)
  {
    // Create a new particle based on the current configuration
    Particle p = this->createParticle();

    // Add the new particle if free memory is available
    size_t particleIndex;
    if (!this->addParticle(p,particleIndex))
      break; //no more free memory for new particles

    // Advect particle some random amount of time between 0 and dt
    this->advectParticle(particleIndex,Math::random(0,dt));
  }
}

void ParticleEmitter::inactivateDyingParticles(float dt)
{
  for(std::list<size_t>::iterator iter = mActiveParticles.begin(); iter != mActiveParticles.end();)
  {
    size_t particleIndex = *iter;

    // Kill particles that will not survive this time step
    if((mParticles[particleIndex].lifeLeft - dt) < 0.f)
    {
      //Delete the dying particle
      iter=mActiveParticles.erase(iter);
      mInactiveParticles.push_back(particleIndex);
      mForceParticles.erase(particleIndex);
    }
    else
      ++iter;
  }
}

void ParticleEmitter::advectActiveParticles(float dt)
{
  for(std::list<size_t>::iterator iter = mActiveParticles.begin(); iter != mActiveParticles.end(); ++iter)
    this->advectParticle(*iter,dt);
}

void ParticleEmitter::advectParticle(size_t particleIndex, const float dt)
{
  float maxLambda, speed;
  Vec3 normal, forces, offset;
  Mat4 M;

  float timeLeft = dt;
  int stepCount=0;
  Particle &p = mParticles[particleIndex];

  // Intersecting with geometry will result in multiple sub steps
  // It is therefore useful to bound the number of them for consistent performance
  // even though it is physically wrong
  while (stepCount <mConfig.maxStepIterations && timeLeft > 0.f)
  {
    // Sum the particle forces (including gravity)
    forces = sumParticleForces(particleIndex);

    // Update the velocity vector
    p.velocity +=forces*timeLeft;

    speed = p.velocity.length();

    //Perform a raycast to see whether the particle intersects with geometry
    maxLambda = speed*timeLeft+mConfig.radius;
    Ray ray(p.position,p.velocity);

    //If no collision geometry is present the intersection object is 0
    std::shared_ptr<RayIntersection> intersection = mCollisionScene ? mCollisionScene->closestIntersection(ray,maxLambda) : nullptr;

    //If the trajectory intersects it gets a little more complicated
    if(intersection)
    { 
      //Flip back-facing normals
      normal = intersection->normal();
      normal = dot(normal,p.velocity) > 0 ? -normal : normal;

      //Compute the time until the particle intersects
      maxLambda=intersection->lambda();
      timeLeft -= maxLambda/speed;

      //Compute an offset position slightly above the collision point
      offset = (normal*mConfig.radius);
      p.position=intersection->position()+offset;

      // Reflect velocity vector at surface normal
      p.velocity = reflect(p.velocity,normal).normalize();

      // Add some deviation to the reflected vector
      p.velocity.normalize();
      Vec3 displace = Math::sampleDirectionPhongLobe(p.velocity,mConfig.reflectDeviationExponent);
      p.velocity = (p.velocity + displace).normalize();
      
      // The particle should lose energy when colliding,
      // This is realized by dampening with coefficients 0<coeff<1
      p.velocity *= speed*Math::random(mConfig.reflectVelocityDampeningSpan[0],
        mConfig.reflectVelocityDampeningSpan[1]);
    }
    //This is the easy case, no intersection with collision geometry :)
    else
    {
      // The particle is displaced by the velocity over time
      p.position+=p.velocity*timeLeft;
      timeLeft = 0;
    }
    ++stepCount;
  }

  //subtract life
  p.lifeLeft-=dt;
}

Vec3 ParticleEmitter::sumParticleForces(size_t particleIndex)
{
  Vec3 forces = mConfig.gravity;
  for(ForceParticleMap::const_iterator iter = mForceParticles.begin();iter != mForceParticles.end();++iter)
  {
    // Only compute the force for other particles
    // A particle does not exert force upon itself
    if(iter->first != particleIndex)
      forces+=iter->second->computeForce(mParticles[iter->first],mParticles[particleIndex].position);
  }
  return forces;
}

void ParticleEmitter::fireworkStep(float dt)
{
  //Iterate over all active particles
  for(std::list<size_t>::iterator iter = mActiveParticles.begin(); iter != mActiveParticles.end(); ++iter)
  {
    Particle &p = mParticles[*iter];

    //TODO: fireworks
    // A fireworks is based on particles that emit new particles
    // when exploding / dying.

    // Note: you may only want to handle those particles that have been
    // emitted by the emitter and not from you
    // For this you can check if the variable
    // particle.isSpecialParticle == 0.f

    // E.g. for new particles
    // Particle newParticle = createParticle();
    // do something ..
    // newParticle.isSpecialParticle = p.isSpecialParticle+10.f;
    // size_t newParticleIndex;
    // this->addParticle(newParticle,newParticleIndex);

    // Note: you can obtain uniform samples of a new using
    // Math::sampleUnitSphere()

    // Note: do not explicitly remove Particle p in this loop
    // If you want to get rid off it, call p.lifeLeft=0;
  }
}




} //namespace ogl
