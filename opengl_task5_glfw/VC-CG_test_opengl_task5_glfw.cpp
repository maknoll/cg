#include "OpenGL.hpp"
#include <memory>
#include "IndexedTriangleIO.hpp"
#include "TriangleGeometry.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "ParticleEmitter.hpp"
#include "CollisionGeometry.hpp"
#include "CollisionScene.hpp"
#include "Particle.hpp"

std::string gDataPath= ""; ///< The path pointing to the resources (OBJ, shader)
enum SceneChoice
{
  SceneRoom,
  ScenePlanes,
  SceneFireworks
};
SceneChoice gScene=ScenePlanes;

int gWidth = 800;  ///< The width of the OpenGL window
int gHeight = 600; ///< The height of the OpenGL window

std::shared_ptr<ogl::ShaderProgram> gShaderProgram;        ///< Phong shader handle
std::shared_ptr<ogl::Camera> gCamera;                      ///< Orbit camera
std::shared_ptr<ogl::TriangleGeometry> gAxisArrows[3];     ///< XYZ (RGB) coordinate widget
ogl::Vec3 gLight0(0,0,0);
ogl::Vec3 gLight1(0,0,0);
ogl::Vec3 gLight2(0,0,0);
ogl::Vec3 gBackgroundColor(0.1f,0.1f,0.1f); ///< The OpenGL background color

std::shared_ptr<ogl::ShaderProgram> gParticleShaderProgram; ///< Billboard shader handle
std::shared_ptr<ogl::ParticleEmitter>  gParticleEmitter;

std::shared_ptr<ogl::CollisionScene> gCollisionScene;
std::vector<std::shared_ptr<ogl::CollisionGeometry>> gCollisionPlanes;
std::shared_ptr<ogl::CollisionGeometry> gCollisionRoom;
size_t gNumCollisionPlanes=5;

int oldWheelDirection=0;

// Display the particles
void displayParticles()
{
  // Use the Phong shader
  glUseProgram(gParticleShaderProgram->handle());

  // Upload possibly changed camera setup (e.g. camera eye) to GPU
  gCamera->updateUniforms();

  // Bind camera uniform buffer
  gCamera->bind(gParticleShaderProgram->handle(),0,"ub_Camera");

  // Upload the new data to the GPU
  gParticleEmitter->updateUniforms();
  gParticleEmitter->step();

  // Bind model uniform buffer
  gParticleEmitter->bind(gParticleShaderProgram->handle(),1,"ub_Particle");

  // Bind triangle geometry
  glBindVertexArray(gParticleEmitter->handle());

  // Draw the indexed triangle set
  glDrawArrays(GL_POINTS, 0,gParticleEmitter->numParticles());
}

// Display the coordinate axes
void displayAxisArrows()
{
  // red arrow   = x-axis
  // green arrow = y-axis
  // blue arrow  = z-axis
  for(size_t i=0;i<3;++i)
  {
    // Upload the new data to the GPU
    gAxisArrows[i]->updateUniforms();

    // Bind model uniform buffer
    gAxisArrows[i]->bind(gShaderProgram->handle(),1,"ub_Geometry");

    // Bind triangle geometry
    glBindVertexArray(gAxisArrows[i]->handle());

    // Draw the indexed triangle set
    glDrawElements(GL_TRIANGLES, gAxisArrows[i]->numIndices(),GL_UNSIGNED_INT, (void*)0);
  }
}


// Display the collision room
void displayCollisionRoom()
{
  // Upload the new data to the GPU
  gCollisionRoom->updateUniforms();

  // Bind model uniform buffer
  gCollisionRoom->bind(gShaderProgram->handle(),1,"ub_Geometry");

  // Bind triangle geometry
  glBindVertexArray(gCollisionRoom->handle());

  // Draw the indexed triangle set
  glDrawElements(GL_TRIANGLES, gCollisionRoom->numIndices(),GL_UNSIGNED_INT, (void*)0);
}

// Display the ground plane
void displayCollisionPlanes()
{
  for(size_t i=0;i<gCollisionPlanes.size();++i)
  {
    std::shared_ptr<ogl::CollisionGeometry> geom = gCollisionPlanes[i];
    // Upload the new data to the GPU
    geom->updateUniforms();

    // Bind model uniform buffer
    geom->bind(gShaderProgram->handle(),1,"ub_Geometry");

    // Bind triangle geometry
    glBindVertexArray(geom->handle());

    // Draw the indexed triangle set
    glDrawElements(GL_TRIANGLES, geom->numIndices(),GL_UNSIGNED_INT, (void*)0);
  }
}

void displayCB()
{
  // Clear frame and depth buffers
  glClearColor(gBackgroundColor[0],gBackgroundColor[1],gBackgroundColor[2], 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Use the Phong shader
  glUseProgram(gShaderProgram->handle());

  // Upload possibly changed camera setup (e.g. camera eye) to GPU
  gCamera->updateUniforms();

  // Bind camera uniform buffer
  gCamera->bind(gShaderProgram->handle(),0,"ub_Camera");

  // Render the triangle scene content
  displayAxisArrows();
  switch (gScene)
  {
  case SceneRoom:
    displayCollisionRoom();
    break;
  case ScenePlanes:
    displayCollisionPlanes();
    break;
  case SceneFireworks: //no collision geometry is present
    break;
  }
  // Unbind the Phong shader
  glUseProgram(0);

  //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
  // use that for smoke see: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch23.html

  //use additive blending for particles
  glBlendFunc(GL_SRC_ALPHA, GL_ONE); 

  //deactivate depth write, but perform depth test!
  glDepthMask (GL_FALSE);

  displayParticles();

  glDepthMask (GL_TRUE); //must be true before swapping a depth buffer

  // Swap the rendering target with the frame buffer shown on the monitor
  glfwSwapBuffers();

  // Hopefully there hasn't been any mistake?
  ogl::printOpenGLError();
}

// Initialize the arrow geometry
// Load the OBJ files, apply materials and transformations
bool initAxisArrows()
{
  ogl::IndexedTriangleIO io;
  io.loadFromOBJ(gDataPath+"arrowZ.obj");
  if(io.vertexNormals().empty())
  {
    std::cerr<<"OBJ model needs vertex normals!"<<std::endl;
    return false;
  }

  std::cerr<<"Loaded "<<io.vertexPositions().size()<< " vertices, "<<
    io.triangleIndices().size()/3<<" triangles.\n\n";

  // Create the blue z-axis arrow
  gAxisArrows[0]= std::make_shared<ogl::TriangleGeometry>();
  gAxisArrows[0]->init(io.vertexPositions(),io.vertexNormals(),io.triangleIndices());
  gAxisArrows[0]->setMaterial(50.f,ogl::Vec3(0,0,1));
  gAxisArrows[0]->setLightPosition0(gLight0);
  gAxisArrows[0]->setLightPosition1(gLight1);
  gAxisArrows[0]->setLightPosition2(gLight2);

  // Create the green y-axis arrow
  gAxisArrows[1]= std::make_shared<ogl::TriangleGeometry>();
  gAxisArrows[1]->initInstance(gAxisArrows[0]);
  gAxisArrows[1]->setMaterial(50.f,ogl::Vec3(0,1,0));
  // Rotate about x-axis by -90 degrees
  gAxisArrows[1]->modelMatrix().rotate(-90.f,ogl::Vec3(1,0,0));
  gAxisArrows[1]->setLightPosition0(gLight0);
  gAxisArrows[1]->setLightPosition1(gLight1);
  gAxisArrows[1]->setLightPosition2(gLight2);

  // Create the red x-axis arrow
  gAxisArrows[2]= std::make_shared<ogl::TriangleGeometry>();
  gAxisArrows[2]->setMaterial(50.f,ogl::Vec3(1,0,0));
  gAxisArrows[2]->initInstance(gAxisArrows[0]);
  // Rotate about y-axis by 90 degrees
  gAxisArrows[2]->modelMatrix().rotate(90.f,ogl::Vec3(0,1,0));
  gAxisArrows[2]->setLightPosition0(gLight0);
  gAxisArrows[2]->setLightPosition1(gLight1);
  gAxisArrows[2]->setLightPosition2(gLight2);

  return true;
}

bool initScenePlanes()
{
  ogl::IndexedTriangleIO io;
  io.loadFromOBJ(gDataPath+"collisionPlane.obj");

  if(io.vertexNormals().empty())
  {
    std::cerr<<"OBJ model needs vertex normals!"<<std::endl;
    return false;
  }

  std::cerr<<"Loaded "<<io.vertexPositions().size()<< " vertices, "<<
    io.triangleIndices().size()/3<<" triangles.\n\n";

  gCollisionScene = std::make_shared<ogl::CollisionScene>();
  for(size_t i=0;i<gNumCollisionPlanes;++i)
  {
    std::shared_ptr<ogl::CollisionGeometry> geom= std::make_shared<ogl::CollisionGeometry>();
    if(i==0)
      geom->init(io.vertexPositions(),io.vertexNormals(),io.triangleIndices());
    else
      geom->initInstance(gCollisionPlanes[0]);

    geom->setMaterial(100.f,ogl::Vec3(0.2f,0.5f,1.0f));
    geom->setLightPosition0(gLight0);
    geom->setLightPosition1(gLight1);
    geom->setLightPosition2(gLight2);
    gCollisionPlanes.push_back(geom);
    gCollisionScene->addGeometry(geom);
  }


  //Do no change this one
  gCollisionPlanes[0]->modelMatrix().rotate(-30,0,1,0);
  gCollisionPlanes[0]->modelMatrix().translate(10,0,3);
  gCollisionPlanes[0]->setMaterial(100.f,ogl::Vec3(1.0f,0.2f,0.2f)); //red plane

  //Setup the transformations of the three blue collision planes
  //TODO: find rotations and translations (no scalings!) such that
  //the particle ray hits
  // *first the red plane
  // *then three blue planes
  // *and finally the green plane
  gCollisionPlanes[1]->modelMatrix().translate(0,0,0);
  gCollisionPlanes[2]->modelMatrix().translate(1.1f,0,0);
  gCollisionPlanes[3]->modelMatrix().translate(2.2f,0,0);

  //Do no change this one
  gCollisionPlanes[4]->modelMatrix().translate(10,0,-6);
  gCollisionPlanes[4]->setMaterial(100.f,ogl::Vec3(0.2f,1.0f,0.2f)); //green plane

  //Initialize the particle emitter
  gParticleEmitter = std::make_shared<ogl::ParticleEmitter>();
  gParticleEmitter->setCollisionScene(gCollisionScene);
  ogl::ParticleEmitter::Configuration &c = gParticleEmitter->configuration();
  c.maxNumParticles=10000;
  c.spawnrate=200;
  c.radius=0.1f;
  c.lifeSpan=ogl::Vec2(8,8);
  c.position=ogl::Vec3(-5,0,0);
  c.mainDirection=ogl::Vec3(1,0,1.2f);
  c.velocitySpan=ogl::Vec2(13.5f,13.5f);
  c.randomDirectionExponent=1000000;
  c.reflectDeviationExponent = 1000000;
  c.reflectVelocityDampeningSpan = ogl::Vec2(0.9f,0.9f);
  c.forceParticleLifeSpan = ogl::Vec2(3,4);
  c.enableFireworks=false;

  if(!gParticleEmitter->init())
    return false;

  //Set the camera
  gCamera->setPosition(ogl::Vec3(0,-20,3));
  gCamera->setTarget(ogl::Vec3(5,0,0));

  return true;
}

bool initSceneRoom()
{
  //Load and init the room collision geometry
  ogl::IndexedTriangleIO io;
  io.loadFromOBJ(gDataPath+"collisionRoom.obj");

  if(io.vertexNormals().empty())
  {
    std::cerr<<"OBJ model needs vertex normals!"<<std::endl;
    return false;
  }

  std::cerr<<"Loaded "<<io.vertexPositions().size()<< " vertices, "<<
    io.triangleIndices().size()/3<<" triangles.\n\n";

  //Setup the collision room scene
  gCollisionScene = std::make_shared<ogl::CollisionScene>();
  gCollisionRoom = std::make_shared<ogl::CollisionGeometry>();
  gCollisionRoom->init(io.vertexPositions(),io.vertexNormals(),io.triangleIndices());
  gCollisionRoom->setMaterial(100.f,ogl::Vec3(0.2f,0.5f,1.0f));
  gCollisionRoom->setLightPosition0(gLight0);
  gCollisionRoom->setLightPosition1(gLight1);
  gCollisionRoom->setLightPosition2(gLight2);
  gCollisionScene->addGeometry(gCollisionRoom);

  // Cull the back-facing triangles to allow viewing into the room
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  //Initialize the particle emitter system
  gParticleEmitter = std::make_shared<ogl::ParticleEmitter>();
  gParticleEmitter->setCollisionScene(gCollisionScene);
  ogl::ParticleEmitter::Configuration &c = gParticleEmitter->configuration();

  c.maxNumParticles=20000;
  c.spawnrate=500;
  c.radius=0.1f;
  c.lifeSpan=ogl::Vec2(8,12);
  c.position=ogl::Vec3(0,0,0);
  c.mainDirection=ogl::Vec3(1,0,1.2f);
  c.velocitySpan=ogl::Vec2(13,14);
  c.randomDirectionExponent=500;
  c.reflectDeviationExponent = 5000;
  c.reflectVelocityDampeningSpan = ogl::Vec2(0.5f,0.6f);
  c.forceParticleLifeSpan = ogl::Vec2(3,4);
  c.enableFireworks=false;

  if(!gParticleEmitter->init())
    return false;

  //Set the camera
  gCamera->setPosition(ogl::Vec3(-20,-13,6));
  gCamera->setTarget(ogl::Vec3(3,3,2));
  return true;
}

bool initFireworksScene()
{
  //Set the camera
  gCamera->setPosition(ogl::Vec3(-26,-26,15));
  gCamera->setTarget(ogl::Vec3(0,0,15));

  //Setup the emitter
  gParticleEmitter = std::make_shared<ogl::ParticleEmitter>();
  
  ogl::ParticleEmitter::Configuration &c = gParticleEmitter->configuration();
  c.maxNumParticles=100000;
  c.spawnrate=10;
  c.radius = 0.1f;
  c.position=ogl::Vec3(0,0,0);
  c.mainDirection=ogl::Vec3(0,0,1);
  c.randomDirectionExponent=1000;
  c.velocitySpan=ogl::Vec2(20,25);
  c.lifeSpan = ogl::Vec2(3,4);
  c.enableFireworks=true;

  if(!gParticleEmitter->init())
    return false;

  return true;
}


// Initialize the OpenGL, camera, shader and calls
// geometry-related init-functions.
bool init()
{
  // Activate depth test to discard fragment that are hidden
  glEnable(GL_DEPTH_TEST);

  // Activate anti-aliasing
  glEnable(GL_MULTISAMPLE);

  // Enable alpha blending
  glEnable(GL_BLEND);

  // Set polygon mode to allow solid front and back-facing triangles
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  // Create camera
  gCamera = std::make_shared<ogl::Camera>(gWidth,gHeight);


  ogl::Mat4 M;
  M.translate(7,0,5);
  gLight0 = M*gLight0;
  M.rotate(120,0,0,1);
  gLight1 = M*gLight1;
  M.rotate(120,0,0,1);
  gLight2 = M*gLight2;

  // Create Phong shader
  gShaderProgram   = std::make_shared<ogl::ShaderProgram>();
  if(!gShaderProgram->init(
    gDataPath+"SolidWirePhong.vs",
    gDataPath+"SolidWirePhong.gs",
    gDataPath+"SolidWirePhong.fs"))
    return false;

  // Create particle billboards shader
  gParticleShaderProgram   = std::make_shared<ogl::ShaderProgram>();
  if(!gParticleShaderProgram->init(
    gDataPath+"ParticleShader.vs",
    gDataPath+"ParticleShader.gs",
    gDataPath+"ParticleShader.fs"))
    return false;

  //Call geometry-related init-functions
  if(!initAxisArrows())
    return false;

  //Initialize the scene geometry and particle emitter configuration
  switch (gScene)
  {
  case SceneRoom:
    if(!initSceneRoom())
      return false;
    break;
  case ScenePlanes:
    if(!initScenePlanes())
      return false;
    break;
  case SceneFireworks:
    if(!initFireworksScene())
      return false;
    break;
  }
  return true;
}

// This callback is called upon resizing the OpenGL window,
// e.g. when maximizing the window
void resizeCB(int width, int height)
{
  gCamera->resize(width,height);
}

// This callback is called when moving the mouse while a button is pressed
void motionCB(int x, int y)
{
  gCamera->mouseMoved(x,y);
}

// This callback is called when a mouse button is pressed or released
void mouseCB(int button, int state)
{
  gCamera->mouseButtonPressed(button,state);
}

// This callback is called when the scroll wheel is used
void wheelCB(int direction)
{
  int diff = direction-oldWheelDirection;
  diff = std::max(-1,std::min(1,diff));
  gCamera->mouseWheelScrolled(diff);
  oldWheelDirection=direction;
}

// This callback is called when a keyboard key is pressed
void keyboardCB(int key, int action)
{
  //for codes see decimal column in ASCII table
  //https://en.wikipedia.org/wiki/ASCII#ASCII_printable_characters
  if(key == 87 && action == GLFW_PRESS) // w key
  {
    std::shared_ptr<ogl::ForceParticle> force = std::make_shared<ogl::PointGravitySource>();
    gParticleEmitter->addForceParticle(force);
  }
}

// Main entry point
int main (int argc, char** argv)
{
  std::cerr<<"Use your mouse to rotate,pan and zoom the camera"<<std::endl;
  std::cerr<<"left mouse button + drag -> rotate"<<std::endl;
  std::cerr<<"middle mouse button + drag -> pan"<<std::endl;
  std::cerr<<"scroll wheel up / down -> zoom in / out"<<std::endl;

  std::cerr<<"\nNote: start your program with the working directory set to the folder "<<
    "containing the source code. Otherwise the shader and OBJ files will not be found. ";
  std::cerr<<"In Visual Studio this can be done by going to PROJECT->Properties->Debugging->Working Directory. "<<
    "Set this path to the absolute source path. E.g. D:/CG/SS13/programming/src/opengl_task5. ";
  std::cerr<<"Alternatively you can set the global variable gDataPath to this path.\n"<<std::endl;

  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    exit( EXIT_FAILURE );
  }

  // Create the OpenGL window
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // 4x AA
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open OpenGL fullscreen window
  if( !glfwOpenWindow( gWidth, gHeight, 0,0,0,0, 16,0, GLFW_WINDOW ) )
  {
    fprintf( stderr, "Failed to open GLFW window\n" );
    glfwTerminate();
    exit( EXIT_FAILURE );
  }

  // Set window title
  glfwSetWindowTitle( "Task4: Transformations" );

  // Disable VSync (we want to get as high FPS as possible!)
  glfwSwapInterval( 0 );

  // Setting this is necessary for core profile (tested with MSVC 2013 x64, Windows 7)
  glewExperimental = GL_TRUE;
  // GLEW wraps all OpenGL functions and extensions
  glewInit();
  glGetError(); //GLEW might cause an 'invalid enum' error, safely ignore it?

  // Print OpenGL context information to console
  ogl::printContextInformation();

  // Perform our initialization (OpenGL states, shader, camera, geometry)
  if(!init())
    return -1;

  // Set the appropriate callback functions
  // Window resize callback function
  glfwSetWindowSizeCallback(resizeCB);
  glfwSetMousePosCallback(motionCB);
  glfwSetMouseButtonCallback(mouseCB);
  glfwSetMouseWheelCallback(wheelCB);
  glfwSetKeyCallback(keyboardCB);

  bool running=true;
  while( running )
  {
    // Draw...
    displayCB();

    // Check if window was closed
    running = running && glfwGetWindowParam( GLFW_OPENED );
  }

  // Terminate OpenGL
  glfwTerminate();
}
