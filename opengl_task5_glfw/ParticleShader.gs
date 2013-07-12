#version 330 core

layout(std140) uniform ub_Camera
{
  mat4 P;
  mat4 V;
  vec3 worldEye;
} Camera;

layout(std140) uniform ub_Particle
{
  float radius;
  float maxLifeTime;
} Particle;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vPosition[1];
in vec3 vVelocity[1];
in float vLifeTime[1];
in float vIsSpecialParticle[1];

flat out vec3 gVelocity;
flat out float gLifeTime;
flat out float gRadius;
flat out float gIsSpecialParticle;
out vec2 gScreenPosition;
out vec2 gScreenCenter;

void main()
{
  gVelocity = vVelocity[0];
  gLifeTime = vLifeTime[0];
  gIsSpecialParticle = vIsSpecialParticle[0];
  
  if(gLifeTime< 0.f)
    return;
  
  gRadius = gIsSpecialParticle == 1.f ? 5.f*Particle.radius : Particle.radius;
  float r = gRadius;
  vec4 p,op;
  p = Camera.V*vec4(vPosition[0],1);
  gScreenCenter = p.xy;
  
  op = p + vec4(-r,r,0,0);
  gScreenPosition = op.xy;
  gl_Position = Camera.P * op;
  EmitVertex(); 
  
  op = p + vec4(-r,-r,0,0);
  gScreenPosition = op.xy;
  gl_Position = Camera.P * op;
  EmitVertex(); 

  op = p + vec4(r,r,0,0);
  gScreenPosition = op.xy;
  gl_Position = Camera.P * op;
  EmitVertex(); 

  op = p + vec4(r,-r,0,0);
  gScreenPosition = op.xy;
  gl_Position = Camera.P * op;
  EmitVertex();   
  EndPrimitive();  
}
 