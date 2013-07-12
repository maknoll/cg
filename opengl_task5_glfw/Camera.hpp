#ifndef CAMERA_HPP_INCLUDE_ONCE
#define CAMERA_HPP_INCLUDE_ONCE

#include "OpenGL.hpp"

namespace ogl
{

// This class implements an arc-ball camera that is defined by
// an eye position, a target location that the camera looks at,
// and an up vector
// The camera data is uploaded to the GPU into a uniform buffer
// and can be modified using the mouse (rotation, pan, zoom)
class Camera
{
public:

  // The constructor sets a glViewport of size width x height
  // with everything visible between the clipping planes defined byzNear and zFar
  Camera(int width, int height, float fovy=45.f, float zNear=0.1f, float zFar=1000.f);

  // The destructor cleans the uniform buffer data
  virtual ~Camera();

  // Set the camera position
  void setPosition(const Vec3 &position) {mEye=position;this->updateUniforms();}

  // Set the target point that the camera looks at
  void setTarget(const Vec3 &target) {mTarget=target; this->updateUniforms();}

  // Set the up vector (note: must NOT coincide with view direction defined by
  // eyePosition - target
  void setUp(const Vec3 &up) {mUp=up; mUp.normalize(); this->updateUniforms();}

  // Computes and returns the view matrix
  Mat4 getViewMatrix() { return getLookAt();}

  // Binds the uniform buffer
  void bind(const GLuint shaderProgram,const GLuint bindingPoint=0, const std::string &blockName="ub_Camera") const;

  // Resizes the glViewport and recomputes projection matrix
  void resize(int width, int height);

  // Called when dragging
  // left mouse button + movement -> rotation
  // middle mouse button + movement -> panning (translation orthogonal to view direction)
  void mouseMoved(int x, int y);

  // Called when pressing or releasing a mouse button
  void mouseButtonPressed(int button, int state);

  // Called when using the mouse wheel for zooming
  void mouseWheelScrolled(int direction);

  // Uploads the projection matrix, the view matrix and the world camera position to the GPU
  void updateUniforms();

private:

  Mat4 getPerspective() const; //< computes the projection matrix
  Mat4 getLookAt() const;      //< computes the view matrix
  GLuint mMatrixUniformBuffer; //< handle for uniform buffer

  Vec3 mEye;                   //< world position of camera
  Vec3 mTarget;                //< world position of looked at point
  Vec3 mUp;                    //< world up vector
  bool mRotationActive;        //< left mouse button state for rotation
  bool mPanningActive;         //< middle mouse button state for panning
  int mWidth;                  //< viewport width
  int mHeight;                 //< viewport height
  float mFOVY;                 //< field of view
  float mZNear;                //< distance to near clipping plane
  float mZFar;                 //< distance to far clipping plane
  int mOldMousePosition[2];    //< mouse position of last mouseMoved() call
};
} //namespace ogl

#endif //CAMERA_HPP_INCLUDE_ONCE