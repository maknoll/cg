#ifndef TRIANGLEGEOMETRY_HPP_INCLUDE_ONCE
#define TRIANGLEGEOMETRY_HPP_INCLUDE_ONCE

#include "OpenGL.hpp"
#include <vector>
#include <memory>

namespace ogl
{

// This class uploads and stores an indexed triangle mesh on the GPU
// It also stores a uniform buffer object for model, and normal matrix, 
// as well as Phong shading related material properties
class TriangleGeometry
{
public:
  TriangleGeometry();
  virtual ~TriangleGeometry();

  // Initialize by a set of vertex positions, vertex normals and triangle indices (starting from 0)
  void init(const std::vector<Vec3>& p, const std::vector<Vec3>& n, const std::vector<unsigned int>& t);

  // Initialize an instance by storing a shared pointer to the original triangle geometry
  void initInstance(std::shared_ptr<TriangleGeometry> original);
  void clear();

  // Returns the handle for the vertex array object (either original or instanced)
  GLuint handle() const;

  // Returns the number of triangle indices (either original or instanced)
  GLsizei numIndices() const;

  // Returns the model matrix
  Mat4& modelMatrix(){return mModelMatrix;}

  // Computes and returns the origin position (modelMatrix*origin)
  Vec3 getPosition() const;

  // Set the light positions
  void setLightPosition0(const Vec3& p) {mLightPosition[0]=Vec4(p,1);}
  void setLightPosition1(const Vec3& p) {mLightPosition[1]=Vec4(p,1);}
  void setLightPosition2(const Vec3& p) {mLightPosition[2]=Vec4(p,1);}
  void setMaterial(float shininess, const Vec3& color, float lineWidth=0.f, const Vec3& lineColor=Vec3(0,0,0));

  // Update data and upload it to the GPU
  void updateUniforms();

  // Bind the uniform buffer object for geometry and shading related properties
  void bind(const GLuint shaderProgram,const GLuint bindingPoint=0, const std::string &blockName="ub_Geometry") const;

private:
  Mat4   mModelMatrix;                          //< The model matrix.
  //<
  GLuint mIndexBuffer;                          //< Handle to the VBO storing triangle indices
  GLuint mPositionBuffer;                       //< Handle to the VBO storing vec3 positions
  GLuint mNormalBuffer;                         //< Handle to the VBO storing vec3 normals
  GLuint mVertexArrayObject;                    //< Handle to the VAO
  GLuint mUniformBuffer;                        //< Handle to the UBO for geometry and Phong properties
  GLsizei mNumIndices;                          //< Number of vertex triangle indices (size == 3*number_of_triangles)
  std::shared_ptr<TriangleGeometry> mInstance;  //< If initialized as instance this is the shared pointer to the original
  //<
  float mShininess;                             //< Material shininess coefficient
  float mLineWidth;                             //< Material line width (for superimposed wireframe)
  Vec4 mColor;                                  //< Material color
  Vec4 mLineColor;                              //< Material line color (for superimposed wireframe)
  Vec4 mLightPosition[3];                       //< Three light positions

  bool   mInitialized;                          //< True if initialized
};
}

#endif //TRIANGLEGEOMETRY_HPP_INCLUDE_ONCE
