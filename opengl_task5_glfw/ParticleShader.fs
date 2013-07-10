#version 330 core

layout(std140) uniform ub_Particle
{
  float radius;
  float maxLifeTime;
} Particle;

in vec2 gScreenPosition;
in vec2 gScreenCenter;
flat in float gRadius;
flat in vec3 gVelocity;
flat in float gLifeTime;
flat in float gIsSpecialParticle;

out vec4 fColor;

void main()
{
  //fade out particles just before they die
  float fadeOutThreshold = 0.99;
  float fadeOut = Particle.maxLifeTime/gLifeTime;
  float alphaFadeOut = smoothstep(0,fadeOut,fadeOutThreshold);

  float fadeDistanceThreshold = 0.3;
  float distance = 1-length(gScreenCenter-gScreenPosition)/gRadius;
  distance = smoothstep(0,fadeDistanceThreshold,distance);
  
  fColor.w = distance*alphaFadeOut;

  vec3 fastColor = vec3(1,0.5,0.5);
  vec3 slowColor = vec3(0.5,0.5,1);
  float colorSpeedThreshold = 20;
  float speed = length(gVelocity);
  float t = smoothstep(0,colorSpeedThreshold,speed);
  fColor.xyz=mix(slowColor,fastColor,t);

  if(gIsSpecialParticle==1.f)
  {
    fColor.xyz=vec3(1,0,0);
  } 
}