#include "OpenGL.hpp"
#include <memory>
#include "IndexedTriangleIO.hpp"
#include "TriangleGeometry.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"


int gWidth = 800;  ///< The width of the OpenGL window
int gHeight = 600; ///< The height of the OpenGL window
size_t gNumDice=6; ///< The number of dice
ogl::Vec3 gBackgroundColor(0.7f,0.7f,0.7f); ///< The OpenGL background color
std::string gDataPath= ""; ///< The path pointing to the resources (OBJ, shader)

std::shared_ptr<ogl::ShaderProgram> gShaderProgram;        ///< Phong shader handle
std::shared_ptr<ogl::Camera> gCamera;                      ///< Orbit camera
std::vector<std::shared_ptr<ogl::TriangleGeometry>> gDice; ///< The dice
std::shared_ptr<ogl::TriangleGeometry> gLightSphere[3];    ///< Three point lights
std::shared_ptr<ogl::TriangleGeometry> gGroundPlane;       ///< The ground plane
std::shared_ptr<ogl::TriangleGeometry> gAxisArrows[3];     ///< XYZ (RGB) coordinate widget
float gLastFrameTime=0.f;                                  ///< rendering time of last frame in seconds


// Display the dice
void displayDice()
{
  for(size_t i=0;i<gDice.size();++i)
  {
    // Set the updated light positions
    std::shared_ptr<ogl::TriangleGeometry> die=gDice[i];
    die->setLightPosition0(gLightSphere[0]->getPosition());
    die->setLightPosition1(gLightSphere[1]->getPosition());
    die->setLightPosition2(gLightSphere[2]->getPosition());

    // Upload the new data to the GPU
    die->updateUniforms();

    // Bind model uniform buffer
    die->bind(gShaderProgram->handle(),1,"ub_Geometry");

    // Bind triangle geometry
    glBindVertexArray(die->handle());

    // Draw the indexed triangle set
    glDrawElements(GL_TRIANGLES, die->numIndices(),GL_UNSIGNED_INT, (void*)0);
  }
};

// Display the coordinate axes
// red arrow   = x-axis
// green arrow = y-axis
// blue arrow  = z-axis
void displayAxisArrows()
{
  for(size_t i=0;i<3;++i)
  {
    // Set the updated light positions
    gAxisArrows[i]->setLightPosition0(gLightSphere[0]->getPosition());
    gAxisArrows[i]->setLightPosition1(gLightSphere[1]->getPosition());
    gAxisArrows[i]->setLightPosition2(gLightSphere[2]->getPosition());

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

// Display the ground plane
void displayGroundPlane()
{
  // Set the updated light positions
  gGroundPlane->setLightPosition0(gLightSphere[0]->getPosition());
  gGroundPlane->setLightPosition1(gLightSphere[1]->getPosition());
  gGroundPlane->setLightPosition2(gLightSphere[2]->getPosition());

  // Upload the new data to the GPU
  gGroundPlane->updateUniforms();

  // Bind model uniform buffer
  gGroundPlane->bind(gShaderProgram->handle(),1,"ub_Geometry");

  // Bind triangle geometry
  glBindVertexArray(gGroundPlane->handle());

  // Draw the indexed triangle set
  glDrawElements(GL_TRIANGLES, gGroundPlane->numIndices(),GL_UNSIGNED_INT, (void*)0);
}

// Display the light spheres indicating the light positions
void displayLightSpheres()
{
  // Update the light positions by rotating 3.6 degrees per second about z-axis (up vector)
  // One full rotation thus takes 100 seconds
  for(int i=0;i<3;++i)
    gLightSphere[i]->modelMatrix().rotate(gLastFrameTime*3.6f,ogl::Vec3(0,0,1));

  // Set the updated light positions
  for(int i=0;i<3;++i)
  {
    gLightSphere[i]->setLightPosition0(gLightSphere[0]->getPosition());
    gLightSphere[i]->setLightPosition1(gLightSphere[1]->getPosition());
    gLightSphere[i]->setLightPosition2(gLightSphere[2]->getPosition());

    // Upload the new data to the GPU
    gLightSphere[i]->updateUniforms();

    // Bind model uniform buffer
    gLightSphere[i]->bind(gShaderProgram->handle(),1,"ub_Geometry");

    // Bind triangle geometry
    glBindVertexArray(gLightSphere[i]->handle());

    // Draw the indexed triangle set
    glDrawElements(GL_TRIANGLES, gLightSphere[i]->numIndices(),GL_UNSIGNED_INT, (void*)0);
  }
}

void displayCB()
{
  // Get the current time (in milliseconds since program start)
  int t0 =glutGet(GLUT_ELAPSED_TIME);

  // Clear frame and depth buffers
  glClearColor(gBackgroundColor[0],gBackgroundColor[1],gBackgroundColor[2], 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set polygon mode to allow solid front and back-facing triangles
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  // Use the Phong shader
  glUseProgram(gShaderProgram->handle());

  // Upload possibly changed camera setup (e.g. camera eye) to GPU
  gCamera->updateUniforms();

  // Bind camera uniform buffer
  gCamera->bind(gShaderProgram->handle(),0,"ub_Camera");

  // Render the scene content
  displayLightSpheres();
  displayAxisArrows();
  displayGroundPlane();
  displayDice();

  // Unbind the Phong shader
  glUseProgram(0);

  // Swap the rendering target with the frame buffer shown on the monitor
  glutSwapBuffers();

  // Hopefully there hasn't been any mistake?
  ogl::printOpenGLError();

  // Compute rendering time in seconds by taking difference to current time
  int t1=glutGet(GLUT_ELAPSED_TIME);
  gLastFrameTime = (t1-t0)/1000.f;

  // Request a redraw to continuously call the display() function
  glutPostRedisplay();
}

// Initialize the dice geometry
// Load the OBJ files, apply materials and transformations
bool initDice()
{
  ogl::IndexedTriangleIO io;
#ifdef _DEBUG
   // This smaller model loads much faster in debug mode
  io.loadFromOBJ(gDataPath+"dieDebug.obj");
#else
  // This large model loads fast only in release mode
  io.loadFromOBJ(gDataPath+"die.obj");
#endif
  if(io.vertexNormals().empty())
  {
    std::cerr<<"OBJ model needs vertex normals!"<<std::endl;
    return false;
  }

  std::cerr<<"Loaded "<<io.vertexPositions().size()<< " vertices, "<<
    io.triangleIndices().size()/3<<" triangles.\n\n";

  // Create the dice
  gDice.resize(gNumDice);

  for(size_t i=0;i<gDice.size();++i)
  {
    gDice[i]= std::make_shared<ogl::TriangleGeometry>();
    // The first die stores the original geometry (indexed triangle set)
    if(i==0)
      gDice[0]->init(io.vertexPositions(),io.vertexNormals(),io.triangleIndices());
    // The other dice are instances of the original geometry (no duplicate geometry stored)
    else
      gDice[i]->initInstance(gDice[0]);

    // Set a bluish material with shininess=100.f
    gDice[i]->setMaterial(100.f,ogl::Vec3(0.2f,0.5f,1.0f));
  }

  // TODO: Transform the dice gDice[0..5] to obtain the result
  // shown in the exercise sheet.

  // You can use the Mat4 transformation functions of the model matrices:
  // modelMatrix().translate(tx,ty,tz);
  // modelMatrix().scale(sy,sy,sz);
  // modelMatrix().rotate(angleInDegree,Vec3(axis_X, axis_Y, axis_Z);

  // Note: the origin of the untransformed cube with dimensions
  // x= -0.5 .. 0.5
  // y= -0.5 .. 0.5
  // z=  0.0 .. 1.0
  // is (0,0,0) is the center of the bottom face (with number 1 on it)

  //TODO: Replace these simple transformations

  //gDice[0] should be the large cube with number 1 facing the camera
  gDice[0]->modelMatrix().translate(3,3,0);

  //gDice[1] should be the cube with number 2 facing the camera
  gDice[1]->modelMatrix().translate(3,3,1);

  //gDice[2] should be the cube with number 3 facing the camera
  gDice[2]->modelMatrix().translate(3,3,2);

  //gDice[3] should be the cube with number 4 facing the camera
  gDice[3]->modelMatrix().translate(3,3,3);

  //gDice[4] should be the cube with number 5 facing the camera
  gDice[4]->modelMatrix().translate(3,3,4);


  // Hint for gDice[5] that stands on the tip showing number 6
  // the rotation that is performed on this die is equivalent to the rotation
  // of the vector (1,1,1)^T onto the z-axis (0,0,1).
  // It is helpful to compute this transformation on a sheet of paper.

  //gDice[5] should be the cube with number 6 facing the camera
  gDice[5]->modelMatrix().translate(3,3,5);

  return true;
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

  // Create the green y-axis arrow
  gAxisArrows[1]= std::make_shared<ogl::TriangleGeometry>();
  gAxisArrows[1]->initInstance(gAxisArrows[0]);
  gAxisArrows[1]->setMaterial(50.f,ogl::Vec3(0,1,0));
  // Rotate about x-axis by -90 degrees
  gAxisArrows[1]->modelMatrix().rotate(-90.f,ogl::Vec3(1,0,0));

  // Create the red x-axis arrow
  gAxisArrows[2]= std::make_shared<ogl::TriangleGeometry>();
  gAxisArrows[2]->setMaterial(50.f,ogl::Vec3(1,0,0));
  gAxisArrows[2]->initInstance(gAxisArrows[0]);
  // Rotate about y-axis by 90 degrees
  gAxisArrows[2]->modelMatrix().rotate(90.f,ogl::Vec3(0,1,0));

  return true;
}

// Initialize the ground plane geometry
// Load the OBJ files, apply materials and transformations
bool initGroundPlane()
{
  ogl::IndexedTriangleIO io;
  io.loadFromOBJ(gDataPath+"groundPlane.obj");

  if(io.vertexNormals().empty())
  {
    std::cerr<<"OBJ model needs vertex normals!"<<std::endl;
    return false;
  }

  std::cerr<<"Loaded "<<io.vertexPositions().size()<< " vertices, "<<
    io.triangleIndices().size()/3<<" triangles.\n\n";

  // Create the yellow ground plane
  gGroundPlane= std::make_shared<ogl::TriangleGeometry>();
  gGroundPlane->init(io.vertexPositions(),io.vertexNormals(),io.triangleIndices());

  // Initially the ground plane spans x=-3..3 and y=-3..3
  // Move it such that it spans  x=0..6 and y=0..6
  gGroundPlane->modelMatrix().translate(3,3,0);
  gGroundPlane->setMaterial(100.f,ogl::Vec3(1.0f,0.7f,0.1f));
  return true;
}

// Initialize the light sphere geometry
// Load the OBJ files, apply materials and transformations
bool initLightSpheres()
{
  ogl::IndexedTriangleIO io;
  io.loadFromOBJ(gDataPath+"lightSphere.obj");

  if(io.vertexNormals().empty())
  {
    std::cerr<<"OBJ model needs vertex normals!"<<std::endl;
    return false;
  }

  std::cerr<<"Loaded "<<io.vertexPositions().size()<< " vertices, "<<
    io.triangleIndices().size()/3<<" triangles.\n\n";

  for(int i=0;i<3;++i)
  {
    gLightSphere[i]= std::make_shared<ogl::TriangleGeometry>();
    // The first sphere stores the original geometry (indexed triangle set)
    if(i==0)
      gLightSphere[i]->init(io.vertexPositions(),io.vertexNormals(),io.triangleIndices());
    // The other spheres are instances of the original geometry (no duplicate geometry stored)
    else
      gLightSphere[i]->initInstance(gLightSphere[0]);

    // Move away from the origin
    gLightSphere[i]->modelMatrix().translate(10,0,5);
  }

  // Position three spheres on a circle equidistantly
  //gLightSphere[0]->modelMatrix().rotate(0,ogl::Vec3(0,0,1));
  gLightSphere[1]->modelMatrix().rotate(120,ogl::Vec3(0,0,1));
  gLightSphere[2]->modelMatrix().rotate(240,ogl::Vec3(0,0,1));

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

  // Create camera
  gCamera = std::make_shared<ogl::Camera>(gWidth,gHeight);
  gCamera->setPosition(ogl::Vec3(16,-9,6));
  gCamera->setTarget(ogl::Vec3(3,3,2));

  // Create Phong shader
  gShaderProgram   = std::make_shared<ogl::ShaderProgram>(
    gDataPath+"SolidWirePhong.vs",
    gDataPath+"SolidWirePhong.gs",
    gDataPath+"SolidWirePhong.fs");

  //Call geometry-related init-functions
  if(!initLightSpheres())
    return false;

  if(!initAxisArrows())
    return false;

  if(!initGroundPlane())
    return false;

  if(!initDice())
    return false;


  return true;
}

// This callback is called upon resizing the OpenGL window,
// e.g. when maximizing the window
void resizeCB(int width, int height)
{
  gCamera->resize(width,height);
  glutPostRedisplay();
}

// This callback is called when moving the mouse while a button is pressed
void motionCB(int x, int y)
{
  gCamera->mouseMoved(x,y);
  glutPostRedisplay();
}

// This callback is called when a mouse button is pressed or released
void mouseCB(int button, int state, int x, int y)
{
  gCamera->mouseButtonPressed(button,state,x,y);
  glutPostRedisplay();
}

// This callback is called when the scroll wheel is used
void wheelCB(int wheel, int direction, int x, int y)
{
  gCamera->mouseWheelScrolled(wheel,direction,x,y);
  glutPostRedisplay();
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
    "Set this path to the absolute source path. E.g. D:/CG/SS13/programming/src/opengl_task4. ";
  std::cerr<<"Alternatively you can set the global variable gDataPath to this path.\n"<<std::endl;

  // Initialize an OpenGL core profile context with version 3.3
  glutInitContextVersion(3,3);
  glutInitContextFlags(GLUT_DEBUG); //GLUT_DEBUG, GLUT_FORWARD_COMPATIBLE
  glutInitContextProfile(GLUT_CORE_PROFILE); //GLUT_CORE_PROFILE, GLUT_COMPATIBILITY_PROFILE

  // Initialize GLUT and create titled window
  glutInit              (&argc, argv);
  glutInitDisplayMode   (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize    (gWidth,gHeight);
  glutInitWindowPosition(100,100);
  glutCreateWindow      ("Task4: Transformations");



  // Setting this is necessary for core profile (tested with MSVC 2013 x64, Windows 7)
  glewExperimental = GL_TRUE;
  // GLEW wraps all OpenGL functions and extensions
  glewInit();
  glGetError(); //GLEW might cause an 'invalid enum' error, safely ignore it?

  // Print OpenGL context information to console
  ogl::printContextInformation();

  // Set the appropriate callback functions
  glutDisplayFunc       (displayCB);
  glutReshapeFunc(resizeCB);
  glutMotionFunc(motionCB);
  glutMouseFunc(mouseCB);
  glutMouseWheelFunc(wheelCB);

  // Perform our initialization (OpenGL states, shader, camera, geometry)
  if(!init())
    return -1;

  // Hopefully, nothing went wrong?
  ogl::printOpenGLError();

  // Enter the GLUT main loop
  // This can be conceptually thought of such a loop that is never
  // left while the program is running
  // enter GLUT main loop
  // while (true)
  // {
  //   determine whether any keyboard key has been pressed
  //   call keyboard callbacks
  //
  //   determine whether mouse has been used (state changed)
  //   call mouse callbacks
  //
  //
  //   idle or call the display function
  //
  // }
  // exit GLUT main loop and return to main-function
  glutMainLoop();

  //The loop exited, end the program.
  return 0;
}
