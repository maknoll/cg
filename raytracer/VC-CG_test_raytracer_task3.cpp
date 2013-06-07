#include "Image.hpp"
#include "PerspectiveCamera.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Raytracer.hpp"
#include "Light.hpp"
#include "DiffuseMaterial.hpp"
#include "Plane.hpp"
#include "Math.hpp"
#include "Triangle.hpp"
#include "Timer.hpp"

#include "BezierPatchMesh.hpp"
#include "CheckerMaterial.hpp"

#include "BVHIndexedTriangleMesh.hpp"
#include "PhongMaterial.hpp"

//Creates a cuboid podium
std::shared_ptr<rt::Renderable> makePodium()
{
  rt::real d=0.5; // half side length
  rt::real h=0.2; // height
  std::shared_ptr<rt::TriangleMesh> triMesh = std::make_shared<rt::TriangleMesh>();

  // define vertices for a cuboid around the x-y origin with height h
  rt::Vec3 v[8] ={rt::Vec3(-d,-d,0),rt::Vec3(d,-d,0),rt::Vec3(d,d,0),rt::Vec3(-d,d,0),
                  rt::Vec3(-d,-d,h),rt::Vec3(d,-d,h),rt::Vec3(d,d,h),rt::Vec3(-d,d,h)};

  // add rectangular sides using two triangles each
  // the winding-order is counter-clockwise (or mathematically positive)
  triMesh->addTriangle(v[0],v[1],v[5]); triMesh->addTriangle(v[0],v[5],v[4]); //left 
  triMesh->addTriangle(v[1],v[2],v[6]); triMesh->addTriangle(v[1],v[6],v[5]); //front
  triMesh->addTriangle(v[2],v[3],v[7]); triMesh->addTriangle(v[2],v[7],v[6]); //right
  triMesh->addTriangle(v[3],v[0],v[4]); triMesh->addTriangle(v[3],v[4],v[7]); //back
  triMesh->addTriangle(v[4],v[5],v[6]); triMesh->addTriangle(v[4],v[6],v[7]); //top

  return triMesh;
}

//Creates a wavy Bezier surface
std::shared_ptr<rt::Renderable> makeBezierWave(size_t numU, size_t numV)
{
  std::shared_ptr<rt::BezierPatchMesh> bezier = std::make_shared<rt::BezierPatchMesh>(3,3,numU,numV);
  bezier->setControlPoint(0,0,rt::Vec3(-0.4,-0.4, 0.3));
  bezier->setControlPoint(1,0,rt::Vec3( 0.0,-0.4, 0.2));
  bezier->setControlPoint(2,0,rt::Vec3( 0.4,-0.4, 0.1));
                            
  bezier->setControlPoint(0,1,rt::Vec3(-0.4, 0.0, 0.0));
  bezier->setControlPoint(1,1,rt::Vec3( 0.0, 0.0,-0.2));
  bezier->setControlPoint(2,1,rt::Vec3( 0.4, 0.0, 0.2));
                             
  bezier->setControlPoint(0,2,rt::Vec3(-0.4, 0.4, 0.3));
  bezier->setControlPoint(1,2,rt::Vec3( 0.0, 0.4, 0.2));
  bezier->setControlPoint(2,2,rt::Vec3( 0.4, 0.4, 0.0));

  // Do not forget to call initialize after adding all control points
  // In order to create the necessary triangles by sampling the parametric surface
  bezier->initialize();
  return bezier;
}

//Creates a Utah teapot (includes material assignment and transformation over to the right podium)
void makeBezierTeapot(std::shared_ptr<rt::Scene> scene,
                      std::shared_ptr<rt::Material> bezierMaterial,
                      size_t numU, size_t numV)
{

  //the original Utah teapot control points
  std::shared_ptr<rt::BezierPatchMesh> bpatch;
  const rt::real b[]={1.4,0.0,2.4,1.4,-0.784,2.4,0.784,-1.4,2.4,0.0,-1.4,2.4,1.3375,0.0,2.53125,1.3375,-0.749,2.53125,0.749,-1.3375,2.53125,0.0,-1.3375,2.53125,
    1.4375,0.0,2.53125,1.4375,-0.805,2.53125,0.805,-1.4375,2.53125,0.0,-1.4375,2.53125,1.5,0.0,2.4,1.5,-0.84,2.4,0.84,-1.5,2.4,0.0,-1.5,2.4,0.0,-1.4,2.4,-0.784,
    -1.4,2.4,-1.4,-0.784,2.4,-1.4,0.0,2.4,0.0,-1.3375,2.53125,-0.749,-1.3375,2.53125,-1.3375,-0.749,2.53125,-1.3375,0.0,2.53125,0.0,-1.4375,2.53125,-0.805,-1.4375,
    2.53125,-1.4375,-0.805,2.53125,-1.4375,0.0,2.53125,0.0,-1.5,2.4,-0.84,-1.5,2.4,-1.5,-0.84,2.4,-1.5,0.0,2.4,-1.4,0.0,2.4,-1.4,0.784,2.4,-0.784,1.4,2.4,0.0,1.4,
    2.4,-1.3375,0.0,2.53125,-1.3375,0.749,2.53125,-0.749,1.3375,2.53125,0.0,1.3375,2.53125,-1.4375,0.0,2.53125,-1.4375,0.805,2.53125,-0.805,1.4375,2.53125,0.0,
    1.4375,2.53125,-1.5,0.0,2.4,-1.5,0.84,2.4,-0.84,1.5,2.4,0.0,1.5,2.4,0.0,1.4,2.4,0.784,1.4,2.4,1.4,0.784,2.4,1.4,0.0,2.4,0.0,1.3375,2.53125,0.749,1.3375,
    2.53125,1.3375,0.749,2.53125,1.3375,0.0,2.53125,0.0,1.4375,2.53125,0.805,1.4375,2.53125,1.4375,0.805,2.53125,1.4375,0.0,2.53125,0.0,1.5,2.4,0.84,1.5,2.4,1.5,
    0.84,2.4,1.5,0.0,2.4,1.5,0.0,2.4,1.5,-0.84,2.4,0.84,-1.5,2.4,0.0,-1.5,2.4,1.75,0.0,1.875,1.75,-0.98,1.875,0.98,-1.75,1.875,0.0,-1.75,1.875,2.0,0.0,1.35,2.0,
    -1.12,1.35,1.12,-2.0,1.35,0.0,-2.0,1.35,2.0,0.0,0.9,2.0,-1.12,0.9,1.12,-2.0,0.9,0.0,-2.0,0.9,0.0,-1.5,2.4,-0.84,-1.5,2.4,-1.5,-0.84,2.4,-1.5,0.0,2.4,0.0,-1.75,
    1.875,-0.98,-1.75,1.875,-1.75,-0.98,1.875,-1.75,0.0,1.875,0.0,-2.0,1.35,-1.12,-2.0,1.35,-2.0,-1.12,1.35,-2.0,0.0,1.35,0.0,-2.0,0.9,-1.12,-2.0,0.9,-2.0,-1.12,
    0.9,-2.0,0.0,0.9,-1.5,0.0,2.4,-1.5,0.84,2.4,-0.84,1.5,2.4,0.0,1.5,2.4,-1.75,0.0,1.875,-1.75,0.98,1.875,-0.98,1.75,1.875,0.0,1.75,1.875,-2.0,0.0,1.35,-2.0,1.12,
    1.35,-1.12,2.0,1.35,0.0,2.0,1.35,-2.0,0.0,0.9,-2.0,1.12,0.9,-1.12,2.0,0.9,0.0,2.0,0.9,0.0,1.5,2.4,0.84,1.5,2.4,1.5,0.84,2.4,1.5,0.0,2.4,0.0,1.75,1.875,0.98,
    1.75,1.875,1.75,0.98,1.875,1.75,0.0,1.875,0.0,2.0,1.35,1.12,2.0,1.35,2.0,1.12,1.35,2.0,0.0,1.35,0.0,2.0,0.9,1.12,2.0,0.9,2.0,1.12,0.9,2.0,0.0,0.9,2.0,0.0,0.9,
    2.0,-1.12,0.9,1.12,-2.0,0.9,0.0,-2.0,0.9,2.0,0.0,0.45,2.0,-1.12,0.45,1.12,-2.0,0.45,0.0,-2.0,0.45,1.5,0.0,0.225,1.5,-0.84,0.225,0.84,-1.5,0.225,0.0,-1.5,0.225,
    1.5,0.0,0.15,1.5,-0.84,0.15,0.84,-1.5,0.15,0.0,-1.5,0.15,0.0,-2.0,0.9,-1.12,-2.0,0.9,-2.0,-1.12,0.9,-2.0,0.0,0.9,0.0,-2.0,0.45,-1.12,-2.0,0.45,-2.0,-1.12,0.45,
    -2.0,0.0,0.45,0.0,-1.5,0.225,-0.84,-1.5,0.225,-1.5,-0.84,0.225,-1.5,0.0,0.225,0.0,-1.5,0.15,-0.84,-1.5,0.15,-1.5,-0.84,0.15,-1.5,0.0,0.15,-2.0,0.0,0.9,-2.0,1.12,
    0.9,-1.12,2.0,0.9,0.0,2.0,0.9,-2.0,0.0,0.45,-2.0,1.12,0.45,-1.12,2.0,0.45,0.0,2.0,0.45,-1.5,0.0,0.225,-1.5,0.84,0.225,-0.84,1.5,0.225,0.0,1.5,0.225,-1.5,0.0,
    0.15,-1.5,0.84,0.15,-0.84,1.5,0.15,0.0,1.5,0.15,0.0,2.0,0.9,1.12,2.0,0.9,2.0,1.12,0.9,2.0,0.0,0.9,0.0,2.0,0.45,1.12,2.0,0.45,2.0,1.12,0.45,2.0,0.0,0.45,0.0,1.5,
    0.225,0.84,1.5,0.225,1.5,0.84,0.225,1.5,0.0,0.225,0.0,1.5,0.15,0.84,1.5,0.15,1.5,0.84,0.15,1.5,0.0,0.15,-1.6,0.0,2.025,-1.6,-0.3,2.025,-1.5,-0.3,2.25,-1.5,0.0,
    2.25,-2.3,0.0,2.025,-2.3,-0.3,2.025,-2.5,-0.3,2.25,-2.5,0.0,2.25,-2.7,0.0,2.025,-2.7,-0.3,2.025,-3.0,-0.3,2.25,-3.0,0.0,2.25,-2.7,0.0,1.8,-2.7,-0.3,1.8,-3.0,
    -0.3,1.8,-3.0,0.0,1.8,-1.5,0.0,2.25,-1.5,0.3,2.25,-1.6,0.3,2.025,-1.6,0.0,2.025,-2.5,0.0,2.25,-2.5,0.3,2.25,-2.3,0.3,2.025,-2.3,0.0,2.025,-3.0,0.0,2.25,-3.0,
    0.3,2.25,-2.7,0.3,2.025,-2.7,0.0,2.025,-3.0,0.0,1.8,-3.0,0.3,1.8,-2.7,0.3,1.8,-2.7,0.0,1.8,-2.7,0.0,1.8,-2.7,-0.3,1.8,-3.0,-0.3,1.8,-3.0,0.0,1.8,-2.7,0.0,1.575,
    -2.7,-0.3,1.575,-3.0,-0.3,1.35,-3.0,0.0,1.35,-2.5,0.0,1.125,-2.5,-0.3,1.125,-2.65,-0.3,0.9375,-2.65,0.0,0.9375,-2.0,0.0,0.9,-2.0,-0.3,0.9,-1.9,-0.3,0.6,-1.9,0.0,
    0.6,-3.0,0.0,1.8,-3.0,0.3,1.8,-2.7,0.3,1.8,-2.7,0.0,1.8,-3.0,0.0,1.35,-3.0,0.3,1.35,-2.7,0.3,1.575,-2.7,0.0,1.575,-2.65,0.0,0.9375,-2.65,0.3,0.9375,-2.5,0.3,
    1.125,-2.5,0.0,1.125,-1.9,0.0,0.6,-1.9,0.3,0.6,-2.0,0.3,0.9,-2.0,0.0,0.9,1.7,0.0,1.425,1.7,-0.66,1.425,1.7,-0.66,0.6,1.7,0.0,0.6,2.6,0.0,1.425,2.6,-0.66,1.425,
    3.1,-0.66,0.825,3.1,0.0,0.825,2.3,0.0,2.1,2.3,-0.25,2.1,2.4,-0.25,2.025,2.4,0.0,2.025,2.7,0.0,2.4,2.7,-0.25,2.4,3.3,-0.25,2.4,3.3,0.0,2.4,1.7,0.0,0.6,1.7,0.66,
    0.6,1.7,0.66,1.425,1.7,0.0,1.425,3.1,0.0,0.825,3.1,0.66,0.825,2.6,0.66,1.425,2.6,0.0,1.425,2.4,0.0,2.025,2.4,0.25,2.025,2.3,0.25,2.1,2.3,0.0,2.1,3.3,0.0,2.4,3.3,
    0.25,2.4,2.7,0.25,2.4,2.7,0.0,2.4,2.7,0.0,2.4,2.7,-0.25,2.4,3.3,-0.25,2.4,3.3,0.0,2.4,2.8,0.0,2.475,2.8,-0.25,2.475,3.525,-0.25,2.49375,3.525,0.0,2.49375,2.9,0.0,
    2.475,2.9,-0.15,2.475,3.45,-0.15,2.5125,3.45,0.0,2.5125,2.8,0.0,2.4,2.8,-0.15,2.4,3.2,-0.15,2.4,3.2,0.0,2.4,3.3,0.0,2.4,3.3,0.25,2.4,2.7,0.25,2.4,2.7,0.0,2.4,
    3.525,0.0,2.49375,3.525,0.25,2.49375,2.8,0.25,2.475,2.8,0.0,2.475,3.45,0.0,2.5125,3.45,0.15,2.5125,2.9,0.15,2.475,2.9,0.0,2.475,3.2,0.0,2.4,3.2,0.15,2.4,2.8,0.15,
    2.4,2.8,0.0,2.4,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.0,3.15,0.8,0.0,3.15,0.8,-0.45,3.15,0.45,-0.8,3.15,0.0,-0.8,3.15,0.0,0.0,2.85,0.0,0.0,2.85,0.0,0.0,
    2.85,0.0,0.0,2.85,0.2,0.0,2.7,0.2,-0.112,2.7,0.112,-0.2,2.7,0.0,-0.2,2.7,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.0,3.15,0.0,-0.8,3.15,-0.45,-0.8,3.15,-0.8,
    -0.45,3.15,-0.8,0.0,3.15,0.0,0.0,2.85,0.0,0.0,2.85,0.0,0.0,2.85,0.0,0.0,2.85,0.0,-0.2,2.7,-0.112,-0.2,2.7,-0.2,-0.112,2.7,-0.2,0.0,2.7,0.0,0.0,3.15,0.0,0.0,3.15,
    0.0,0.0,3.15,0.0,0.0,3.15,-0.8,0.0,3.15,-0.8,0.45,3.15,-0.45,0.8,3.15,0.0,0.8,3.15,0.0,0.0,2.85,0.0,0.0,2.85,0.0,0.0,2.85,0.0,0.0,2.85,-0.2,0.0,2.7,-0.2,0.112,2.7,
    -0.112,0.2,2.7,0.0,0.2,2.7,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.0,3.15,0.0,0.8,3.15,0.45,0.8,3.15,0.8,0.45,3.15,0.8,0.0,3.15,0.0,0.0,2.85,0.0,0.0,2.85,0.0,
    0.0,2.85,0.0,0.0,2.85,0.0,0.2,2.7,0.112,0.2,2.7,0.2,0.112,2.7,0.2,0.0,2.7,0.2,0.0,2.7,0.2,-0.112,2.7,0.112,-0.2,2.7,0.0,-0.2,2.7,0.4,0.0,2.55,0.4,-0.224,2.55,
    0.224,-0.4,2.55,0.0,-0.4,2.55,1.3,0.0,2.55,1.3,-0.728,2.55,0.728,-1.3,2.55,0.0,-1.3,2.55,1.3,0.0,2.4,1.3,-0.728,2.4,0.728,-1.3,2.4,0.0,-1.3,2.4,0.0,-0.2,2.7,
    -0.112,-0.2,2.7,-0.2,-0.112,2.7,-0.2,0.0,2.7,0.0,-0.4,2.55,-0.224,-0.4,2.55,-0.4,-0.224,2.55,-0.4,0.0,2.55,0.0,-1.3,2.55,-0.728,-1.3,2.55,-1.3,-0.728,2.55,-1.3,
    0.0,2.55,0.0,-1.3,2.4,-0.728,-1.3,2.4,-1.3,-0.728,2.4,-1.3,0.0,2.4,-0.2,0.0,2.7,-0.2,0.112,2.7,-0.112,0.2,2.7,0.0,0.2,2.7,-0.4,0.0,2.55,-0.4,0.224,2.55,-0.224,0.4,
    2.55,0.0,0.4,2.55,-1.3,0.0,2.55,-1.3,0.728,2.55,-0.728,1.3,2.55,0.0,1.3,2.55,-1.3,0.0,2.4,-1.3,0.728,2.4,-0.728,1.3,2.4,0.0,1.3,2.4,0.0,0.2,2.7,0.112,0.2,2.7,0.2,
    0.112,2.7,0.2,0.0,2.7,0.0,0.4,2.55,0.224,0.4,2.55,0.4,0.224,2.55,0.4,0.0,2.55,0.0,1.3,2.55,0.728,1.3,2.55,1.3,0.728,2.55,1.3,0.0,2.55,0.0,1.3,2.4,0.728,1.3,2.4,1.3,
    0.728,2.4,1.3,0.0,2.4,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.425,0.0,0.0,1.425,0.798,0.0,0.798,1.425,0.0,0.0,1.425,0.0,1.5,0.0,0.075,1.5,0.84,0.075,0.84,
    1.5,0.075,0.0,1.5,0.075,1.5,0.0,0.15,1.5,0.84,0.15,0.84,1.5,0.15,0.0,1.5,0.15,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.425,0.0,-0.798,1.425,0.0,-1.425,
    0.798,0.0,-1.425,0.0,0.0,0.0,1.5,0.075,-0.84,1.5,0.075,-1.5,0.84,0.075,-1.5,0.0,0.075,0.0,1.5,0.15,-0.84,1.5,0.15,-1.5,0.84,0.15,-1.5,0.0,0.15,0.0,0.0,0.0,0.0,0.0,
    0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.425,0.0,0.0,-1.425,-0.798,0.0,-0.798,-1.425,0.0,0.0,-1.425,0.0,-1.5,0.0,0.075,-1.5,-0.84,0.075,-0.84,-1.5,0.075,0.0,-1.5,0.075,-1.5,
    0.0,0.15,-1.5,-0.84,0.15,-0.84,-1.5,0.15,0.0,-1.5,0.15,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-1.425,0.0,0.798,-1.425,0.0,1.425,-0.798,0.0,1.425,0.0,
    0.0,0.0,-1.5,0.075,0.84,-1.5,0.075,1.5,-0.84,0.075,1.5,0.0,0.075,0.0,-1.5,0.15,0.84,-1.5,0.15,1.5,-0.84,0.15,1.5,0.0,0.15};
  rt::Vec3* bb = (rt::Vec3*)&b[0];

  for(int i = 0; i < 32 ; ++i)
  {
    bpatch = std::make_shared<rt::BezierPatchMesh>(4,4,numU,numV);
    bpatch->setControlPoint(0,0,bb[i*16+ 0]);
    bpatch->setControlPoint(1,0,bb[i*16+ 1]);
    bpatch->setControlPoint(2,0,bb[i*16+ 2]);
    bpatch->setControlPoint(3,0,bb[i*16+ 3]);
    bpatch->setControlPoint(0,1,bb[i*16+ 4]);
    bpatch->setControlPoint(1,1,bb[i*16+ 5]);
    bpatch->setControlPoint(2,1,bb[i*16+ 6]);
    bpatch->setControlPoint(3,1,bb[i*16+ 7]);
    bpatch->setControlPoint(0,2,bb[i*16+ 8]);
    bpatch->setControlPoint(1,2,bb[i*16+ 9]);
    bpatch->setControlPoint(2,2,bb[i*16+10]);
    bpatch->setControlPoint(3,2,bb[i*16+11]);
    bpatch->setControlPoint(0,3,bb[i*16+12]);
    bpatch->setControlPoint(1,3,bb[i*16+13]);
    bpatch->setControlPoint(2,3,bb[i*16+14]);
    bpatch->setControlPoint(3,3,bb[i*16+15]);
    bpatch->initialize();
    bpatch->setMaterial(bezierMaterial);
    bpatch->transform().scale(rt::Vec3( 0.15,0.15, 0.15));
    bpatch->transform().rotate(rt::Vec3(0,0,1),-M_PI/3.0);
    bpatch->transform().translate(rt::Vec3(0,0.6,0.2));
    scene->addRenderable(bpatch);
  }
}

std::shared_ptr<rt::Scene> makeTask2Scene()
{
  //The scene holds all renderable objects, lights and a camera.
  std::shared_ptr<rt::Scene>    scene     = std::make_shared<rt::Scene>();

  //Create a perspective camera, looking at the origin (0,0,0).
  //The up-direction is (0,0,1).
  std::shared_ptr<rt::Camera>   camera    = std::make_shared<rt::PerspectiveCamera>();

  //Move the camera eye position to (5,0,2).
  camera->setPosition(rt::Vec3(4,0,1.5));

  //Set Horizontal and Vertical field of view to 45 degrees.
  camera->setFOV(40,40);
  scene->setCamera(camera);

  //Add a point light source positioned at (5,2,6) with a yellow tone
  std::shared_ptr<rt::Light>    light1     = std::make_shared<rt::Light>(rt::Vec3(5,2,4), rt::Vec3(0.6,0.6,0.5));
  scene->addLight(light1);

  // This plane is initialized with default values.
  // point on plane: (0,0,0)
  // plane normal: (0,0,1)
  std::shared_ptr<rt::Plane> plane = std::make_shared<rt::Plane>();
  std::shared_ptr<rt::Material> materialPlane = std::make_shared<rt::DiffuseMaterial>(rt::Vec3(1.0,1.0,1.0));
  plane->setMaterial(materialPlane);
  scene->addRenderable(plane);

  // Create two podiums for bezier surfaces that are placed on the plane
  std::shared_ptr<rt::Renderable> podiumLeft = makePodium();
  std::shared_ptr<rt::Renderable> podiumRight = makePodium();

  // Every renderable object has an attribute called 'transform' which allows you to scale, rotate and translate 
  // the object.  Note that this will not affect your methods 'closestIntersection' and 'anyIntersect' because 
  // rays will be transformed according to the object's transformation before these methods are called.
  // This principle will become apparent within the following weeks. You can assume that a rays coming from an 
  // arbitrary direction intersects with the unit sphere. This is also done for 'sphere2', which has no 
  // transformation (zero translation).
  podiumLeft->transform().translate(rt::Vec3( 0,-0.6, 0));
  podiumRight->transform().translate(rt::Vec3( 0, 0.6, 0));

  //Create several materials: orange (spheres), blue (plane), red (triangle).
  std::shared_ptr<rt::Material> materialPodium  = std::make_shared<rt::DiffuseMaterial>(rt::Vec3(0.2,0.5,0.2));
  podiumLeft->setMaterial(materialPodium);
  podiumRight->setMaterial(materialPodium);
  scene->addRenderable(podiumLeft);
  scene->addRenderable(podiumRight);

  // Create a wavy bezier patch surface
  // The parameters numU and numV control the number of vertices 
  // for the triangle mesh along the respective direction in the
  // parametrization
  // Experiment with larger numbers for a smoother surface!
  std::shared_ptr<rt::Renderable> bezierWave = makeBezierWave(6,6);

  // Position the bezier surface over the left podium
  bezierWave->transform().translate(rt::Vec3( 0,-0.6, 0.3));

  // Apply a checkerboard material
  std::shared_ptr<rt::Material> materialBezier1  = std::make_shared<rt::DiffuseMaterial>(rt::Vec3(0.2,0.5,1.0));
  std::shared_ptr<rt::Material> materialBezier2  = std::make_shared<rt::DiffuseMaterial>(rt::Vec3(1.0,0.7,0.2));
  std::shared_ptr<rt::CheckerMaterial> materialChecker = 
    std::make_shared<rt::CheckerMaterial>(materialBezier1,materialBezier2);

  bezierWave->setMaterial(materialChecker);
  scene->addRenderable(bezierWave);

  // Now create a Utah teapot bezier surface
  // see http://en.wikipedia.org/wiki/Utah_teapot
  // Warning: this significantly increases the rendering time!
  // Expect a rendering time of within minutes for an image resolution of 1024x512
  makeBezierTeapot(scene,materialChecker,8,8);

  return scene;
}

std::shared_ptr<rt::Scene> makeTask3Scene()
{
  std::shared_ptr<rt::Camera>   camera    = std::make_shared<rt::PerspectiveCamera>();
  camera->setPosition(rt::Vec3(5.0,0.0,5.0));
  camera->setFOV(60.0,60.0);

  std::shared_ptr<rt::Scene>    scene     = std::make_shared<rt::Scene>();

  std::shared_ptr<rt::Material> material1 = std::make_shared<rt::PhongMaterial>(rt::Vec3(1.0,0.4,0.1),0.6,1000.0);
  std::shared_ptr<rt::Material> material2 = std::make_shared<rt::PhongMaterial>(rt::Vec3(0.0,0.0,0.0),0.2,1000.0);
  std::shared_ptr<rt::Material> material3 = std::make_shared<rt::PhongMaterial>(rt::Vec3(0.2,0.3,0.8),0.1,  10.0);
  std::shared_ptr<rt::Material> material4 = std::make_shared<rt::PhongMaterial>(rt::Vec3(0.5,0.0,0.0),0.2,  50.0);
  std::shared_ptr<rt::Material> material5 = std::make_shared<rt::PhongMaterial>(rt::Vec3(0.5,0.5,0.5),0.1, 100.0);

  std::shared_ptr<rt::Sphere>   sphere1   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere>   sphere2   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere>   sphere3   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere>   sphere4   = std::make_shared<rt::Sphere>();

  sphere1->transform().scale(rt::Vec3(1,1,1)).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3( 1.1, 1.1,1.1));
  sphere2->transform().scale(rt::Vec3(1,1,1)).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3(-1.1, 1.1,1.1));
  sphere3->transform().scale(rt::Vec3(1,1,1)).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3( 0.0,-1.1,1.1));
  sphere4->transform().scale(rt::Vec3(1,1,1)).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3( 0.0, 0.0,2.0));

  //if your result image is too bright, you can dampen the spectral intensity
  rt::real intensityFactor=1.0;
  std::shared_ptr<rt::Light>    light1     = std::make_shared<rt::Light>(rt::Vec3(  5.0, 2.0,6.0), intensityFactor*rt::Vec3(200,170,150));
  std::shared_ptr<rt::Light>    light2     = std::make_shared<rt::Light>(rt::Vec3(  5.0,-7.0,3.0), intensityFactor*rt::Vec3(200,170,150));
  std::shared_ptr<rt::Light>    light3     = std::make_shared<rt::Light>(rt::Vec3(-10.0, 4.0,5.0), intensityFactor*rt::Vec3(130,160,200));

  std::shared_ptr<rt::Plane>    plane     = std::make_shared<rt::Plane>();

  sphere1->setMaterial(material1);
  sphere2->setMaterial(material2);
  sphere3->setMaterial(material3);
  sphere4->setMaterial(material4);
  plane  ->setMaterial(material5);

  scene->addRenderable(sphere1);
  scene->addRenderable(sphere2);
  scene->addRenderable(sphere3);
  scene->addRenderable(sphere4);
  scene->addRenderable(plane);

  scene->addLight(light1);
  scene->addLight(light2);
  scene->addLight(light3);

  scene->setCamera(camera);
  return scene;
}

std::shared_ptr<rt::Scene> makeMeshScene(std::string fileName)
{
  std::shared_ptr<rt::Camera>   camera    = std::make_shared<rt::PerspectiveCamera>();
  camera->setPosition(rt::Vec3(5.0,0.0,5.0));
  camera->setFOV(60.0,60.0);

  std::shared_ptr<rt::Scene>    scene     = std::make_shared<rt::Scene>();

  std::shared_ptr<rt::Material> material1 = std::make_shared<rt::PhongMaterial>
    (rt::Vec3(1.0,0.4,0.1),0.8,1000.0);
  std::shared_ptr<rt::Material> material2 = std::make_shared<rt::PhongMaterial>
    (rt::Vec3(0.0,0.0,0.0),0.2,1000.0);
  std::shared_ptr<rt::Material> material3 = std::make_shared<rt::PhongMaterial>
    (rt::Vec3(0.2,0.3,0.8),0.1,  10.0);
  std::shared_ptr<rt::Material> material4 = std::make_shared<rt::PhongMaterial>
    (rt::Vec3(1.0,0.7,0.0),0.2,  50.0);
  std::shared_ptr<rt::Material> material5 = std::make_shared<rt::PhongMaterial>
    (rt::Vec3(0.5,0.5,0.5),0.1, 100.0);

  //if your result image is too bright, you can dampen the spectral intensity
  rt::real intensityFactor=1.0;
  std::shared_ptr<rt::Light>    light1     = std::make_shared<rt::Light>(
    rt::Vec3(  5.0, 2.0,6.0), intensityFactor*rt::Vec3(200,170,150));
  std::shared_ptr<rt::Light>    light2     = std::make_shared<rt::Light>(
    rt::Vec3(  5.0,-7.0,3.0), intensityFactor*rt::Vec3(200,170,150));
  std::shared_ptr<rt::Light>    light3     = std::make_shared<rt::Light>(
    rt::Vec3(-10.0, 4.0,5.0), intensityFactor*rt::Vec3(130,160,200));

  std::shared_ptr<rt::Plane>    plane     = std::make_shared<rt::Plane>();
  plane  ->setMaterial(material5);

  scene->addRenderable(plane);

  scene->addLight(light1);
  scene->addLight(light2);
  scene->addLight(light3);

  scene->setCamera(camera);

  std::shared_ptr<rt::BVHIndexedTriangleMesh> mesh = std::make_shared<rt::BVHIndexedTriangleMesh>();
  mesh->loadFromOBJ(fileName);

  std::cout<<"Loaded BVHMesh with "<<mesh->triangleIndices().size()/3<<
    " triangles and "<< mesh->vertexPositions().size()<<" vertices"<<std::endl;
  mesh->setMaterial(material4);
  scene->addRenderable(mesh);

  return scene;
}

int main()
{
  std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(1080,1080);

//  std::shared_ptr<rt::Scene> scene = makeTask2Scene(); //task2 solution with teapot
  std::shared_ptr<rt::Scene> scene = makeTask3Scene(); //task3 with four spheres

  //Your triangle mesh in OBJ format should be oriented around the origin, with side lengths ~= 2 units
//  std::shared_ptr<rt::Scene> scene = makeMeshScene("smooth_rubberduck.obj");

  std::shared_ptr<rt::Raytracer> raytracer = std::make_shared<rt::Raytracer>();
  raytracer->setScene(scene);

  // Note the fastest way to execute your program using Visual Studio is pressing Ctrl+F5
  // in Release mode. This will detach the rendering process from the debugger.

  // The stopwatches commands will measure time between 'tic' and 'toc'. The result is printed to console.
  util::StopWatch s; s.tic();
  raytracer->renderToImage(image);
  s.toc(std::cout);

  // Save the image in TGA format in a relative folder. Depending on your platform this will differ.
  // If you start your program from Microsoft Visual Studio the image will be written to the folder
  // containing the .vcxproj project file.
  // e.g. D:/courses/CG1/raytracer_task3/build64/src/raytracer_dev_task3
  // If you start your program by double-clicking it will be in the same folder as your .exe file
  // e.g. D:/courses/CG1/raytracer_task3/build64/Release
  image->saveToTGA("./VC-CG_test_raytracer_task3");

  return 0;
}
