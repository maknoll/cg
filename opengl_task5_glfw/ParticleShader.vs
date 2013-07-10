#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float lifeTime;
layout(location = 3) in float isSpecialParticle;

out vec3 vPosition;
out vec3 vVelocity;
out float vLifeTime;
out float vIsSpecialParticle;

void main(void)
{
  vPosition = position;
  vVelocity = velocity;
  vLifeTime = lifeTime;
  vIsSpecialParticle= isSpecialParticle;
}