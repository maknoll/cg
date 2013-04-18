#include "Image.hpp"
#include "PerspectiveCamera.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Raytracer.hpp"
#include "Light.hpp"
#include "ConstantMaterial.hpp"
#include "Plane.hpp"
#include "Math.hpp"
#include "Triangle.hpp"
#include "Timer.hpp"

// Duplicate this function and play around with the configuration!
std::shared_ptr<rt::Scene> makeTask1Scene()
{
  //The scene holds all renderable objects, lights and a camera.
  std::shared_ptr<rt::Scene>    scene     = std::make_shared<rt::Scene>();

  //Create a perspective camera, looking at the origin (0,0,0).
  //The up-direction is (0,0,1).
  std::shared_ptr<rt::Camera>   camera    = std::make_shared<rt::PerspectiveCamera>();

  //Move the camera eye position to (5,0,5).
  camera->setPosition(rt::Vec3(5,0,5));

  //Set Horizontal and Vertical field of view to 60 degrees.
  camera->setFOV(60,60);
  scene->setCamera(camera);

  //The scene needs a light for tracing rays, the intensity does not matter though.
  //The point light source is positioned at (5,2,6)
  std::shared_ptr<rt::Light>    light1     = std::make_shared<rt::Light>(rt::Vec3(5,2,6), rt::Vec3(1,1,1));
  scene->addLight(light1);

  //Create several materials: orange (spheres), blue (plane), red (triangle).
  std::shared_ptr<rt::Material> materialSpheres  = std::make_shared<rt::ConstantMaterial>(rt::Vec3(1.0,0.4,0.1));
  std::shared_ptr<rt::Material> materialPlane    = std::make_shared<rt::ConstantMaterial>(rt::Vec3(0.0,0.2,0.7));
  std::shared_ptr<rt::Material> materialTriangle = std::make_shared<rt::ConstantMaterial>(rt::Vec3(0.8,0.0,0.1));

  std::shared_ptr<rt::Sphere> sphere1   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere> sphere2   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere> sphere3   = std::make_shared<rt::Sphere>();

  // Every renderable object has an attribute called 'transform' which allows you to scale, rotate and translate 
  // the object.  Note that this will not affect your methods 'closestIntersection' and 'anyIntersect' because 
  // rays will be transformed according to the object's transformation before these methods are called.
  // This principle will become apparent within the following weeks. You can assume that a rays coming from an 
  // arbitrary direction intersects with the unit sphere. This is also done for 'sphere2', which has no 
  // transformation (zero translation).
  sphere1->transform().scale(rt::Vec3(1  ,  1,1  )).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3( -5,-2, 1));
  sphere2->transform().scale(rt::Vec3(1.5,1.5,1.5)).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3(  0, 1, 1));
  sphere3->transform().scale(rt::Vec3(1  ,1  ,1  )).rotate(rt::Vec3(0,0,1), 0).translate(rt::Vec3( -2, 0, 2));

  // Every renderable object needs a material and must be added to the scene
  sphere1->setMaterial(materialSpheres);
  sphere2->setMaterial(materialSpheres);
  sphere3->setMaterial(materialSpheres);

  scene->addRenderable(sphere1);
  scene->addRenderable(sphere2);
  scene->addRenderable(sphere3);

  // This plane is initialized with default values.
  // point on plane: (0,0,0)
  // plane normal: (0,0,1)
  std::shared_ptr<rt::Plane> plane = std::make_shared<rt::Plane>();
  plane->setMaterial(materialPlane);
  scene->addRenderable(plane);

  // For triangles you need to know the winding order for vertices, because they have one visible side. 
  // In this case it is counter-clockwise from camera view. You can change the winding by creating a 
  // triangle a,c,b. If you do this, the triangle will not be visible from the camera, because it is 
  // facing away from the camera (and toward 'sphere2')-
  rt::Vec3 a(2,1,0);
  rt::Vec3 b(1,0,2);
  rt::Vec3 c(1,2,0);
  std::shared_ptr<rt::Triangle> triangle = std::make_shared<rt::Triangle>(a,b,c);
  triangle->setMaterial(materialTriangle);
  scene->addRenderable(triangle);

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Here, you can verify your handwritten results from the theoretical exercise on intersections.

  // Intersect ray with triangle
  rt::Ray ray(rt::Vec3(5,0,5),rt::Vec3(-1,3/20.0,-1));
  rt::real maxLambda(1000);
  std::shared_ptr<rt::RayIntersection> isect1=  triangle->closestIntersection(ray,maxLambda);
  if(isect1)
  {
    std::cout << "Ray-Triangle Intersection at "<<std::endl;
    std::cout<<isect1->position()<<std::endl;
  }
  else
    std::cout<<"No Ray-Triangle Intersection"<<std::endl;

  // Intersect ray with sphere with c=(0,1,1) and radius r=1.5
  std::shared_ptr<rt::RayIntersection> isect2=  sphere2->closestIntersection(ray,maxLambda);
  if(isect2)
  {
    std::cout << "Ray-Sphere Intersection at "<<std::endl;
    std::cout<<isect2->position()<<std::endl;
  }
  else
    std::cout<<"No Ray-Sphere Intersection"<<std::endl;

  return scene;
}

int main(int argc, char * argv[])
{
  std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(512,512);
  std::shared_ptr<rt::Scene> scene = makeTask1Scene();

  std::shared_ptr<rt::Raytracer> raytracer = std::make_shared<rt::Raytracer>();
  raytracer->setScene(scene);

  // The stopwatches commands will measure time between 'tic' and 'toc'. The result is printed to console.
  util::StopWatch s; s.tic();
  raytracer->renderToImage(image);
  s.toc(std::cout);

  // Save the image in TGA format in a relative folder. Depending on your platform this will differ.
  // If you start your program from Microsoft Visual Studio the image will be written to the folder
  // containing the .vcxproj project file.
  // e.g. D:/courses/CG1/raytracer_task1/build64/src/raytracer_dev_task1
  // If you start your program by double-clicking it will be in the same folder as your .exe file
  // e.g. D:/courses/CG1/raytracer_task1/build64/Release
  image->saveToTGA("./VC-CG_test_raytracer_task1");

  return 0;
}
