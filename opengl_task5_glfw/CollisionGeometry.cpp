#include "CollisionGeometry.hpp"
#include "Intersection.hpp"
namespace ogl
{

CollisionGeometry::CollisionGeometry() : mInitialized(false)
{
  // Initialize default values
  mModelMatrix.setIdentity();

  mShininess = 10.f;
  mLineWidth = 0.f;
  mColor = Vec4(0.5,0.5,0.5,1);
  mLineColor = Vec4(0,0,0,0);
  mLightPosition[0]=Vec4(0,0,0,0);
  mLightPosition[1]=Vec4(0,0,0,0);
  mLightPosition[2]=Vec4(0,0,0,0);
}

CollisionGeometry::~CollisionGeometry()
{
  this->clear();
}

void CollisionGeometry::init(const std::vector<Vec3>& p, const std::vector<Vec3>& n, const std::vector<unsigned int>& t)
{
  this->clear();


  //     Create and copy buffer data for the indexed triangle set
  // 
  mCollisionPositions = p;
  mCollisionNormals = n;
  mCollisionIndices.resize(t.size()/3);
  for(size_t i=0;i<mCollisionIndices.size();++i)
    mCollisionIndices[i]=Vec3i(t[3*i+0],t[3*i+1],t[3*i+2]);

  mCollisionTree.build(p,mCollisionIndices);

  mNumIndices = GLsizei(t.size());

  // Create and copy index buffer on GPU
  glGenBuffers(1, &mIndexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * t.size(), &t[0], GL_STATIC_DRAW);
  ogl::printOpenGLError();

  // Create and copy index buffer on GPU
  glGenBuffers(1, &mPositionBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * p.size(), &p[0], GL_STATIC_DRAW);
  ogl::printOpenGLError();

  // Create and copy index buffer on GPU
  glGenBuffers(1, &mNormalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * n.size(), &n[0], GL_STATIC_DRAW);
  ogl::printOpenGLError();

  // Generate a vertex array object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  //Create uniform buffer
  glGenBuffers(1, &mUniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 56, NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  mInitialized=true;
}

void CollisionGeometry::initInstance(std::shared_ptr<CollisionGeometry> original)
{
  // For instances, only create uniform buffer 
  // Point to the geometry of the original

  this->clear();
  mInstance=original;

  //Create uniform buffer
  glGenBuffers(1, &mUniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 56, NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  mInitialized=true;
}

GLuint CollisionGeometry::handle() const
{
  if(mInstance)
    return mInstance->handle();
  return mVertexArrayObject;
}
GLsizei CollisionGeometry::numIndices() const
{
  if(mInstance)
    return mInstance->numIndices();
  return mNumIndices;
}


void CollisionGeometry::setMaterial(float shininess, const Vec3& color, float lineWidth, const Vec3& lineColor)
{
  mShininess=shininess;
  mColor=Vec4(color,1);
  mLineWidth=lineWidth;
  mLineColor=Vec4(lineColor,1);
}

Vec3 CollisionGeometry::getPosition() const
{
  return mModelMatrix*Vec3(0,0,0);
}

void CollisionGeometry::clear()
{
  if(!mInitialized)
    return;

  if(!mInstance)
  {
    std::vector<Vec3>().swap(mCollisionPositions);
    std::vector<Vec3i>().swap(mCollisionIndices);

    glDeleteBuffers(1,&mIndexBuffer);
    glDeleteBuffers(1,&mPositionBuffer);
    glDeleteBuffers(1,&mNormalBuffer);
    glDeleteVertexArrays(1,&mVertexArrayObject);

    glDeleteBuffers(1,&mUniformBuffer);
  }

  mInstance=0;
  mInitialized=false;
}

void CollisionGeometry::updateUniforms()
{
  mModelMatrixInverse = mModelMatrix;
  mModelMatrixInverse.invert();
  mModelMatrixInverseTransposed =mModelMatrixInverse;
  mModelMatrixInverseTransposed.transpose();
  // Compute the normal matrix
  Mat4 normalMatrix = mModelMatrix.getInverse().transpose();

  // Upload the new geometry and material properties to the GPU
  glBindBuffer(GL_UNIFORM_BUFFER, mUniformBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16,mModelMatrix.ptr());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16,normalMatrix.ptr());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 32, sizeof(float) * 1,&mShininess);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 33, sizeof(float) * 1,&mLineWidth);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 36, sizeof(float) * 4,mColor.ptr());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 40, sizeof(float) * 4,mLineColor.ptr());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 44, sizeof(float) * 4,mLightPosition[0].ptr());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 48, sizeof(float) * 4,mLightPosition[1].ptr());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 52, sizeof(float) * 4,mLightPosition[2].ptr());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::shared_ptr<RayIntersection>
  CollisionGeometry::closestIntersectionModel(const Ray &ray, float maxLambda) const
{
  const BVTree &collisionTree = mInstance ? mInstance->mCollisionTree : mCollisionTree;
  const std::vector<Vec3> &collisionPositions = mInstance ? mInstance->mCollisionPositions : mCollisionPositions;
  const std::vector<Vec3> &collisionNormals = mInstance ? mInstance->mCollisionNormals : mCollisionNormals;
  const std::vector<Vec3i> &collisionIndices = mInstance ? mInstance->mCollisionIndices : mCollisionIndices;
  const std::vector<int> &intersectionCandidates = collisionTree.intersectBoundingBoxes(ray,maxLambda);

  float closestLambda = maxLambda;
  Vec3 closestbary;
  int  closestTri = -1;

  Vec3 bary;
  float lambda;

  for(size_t i=0;i<intersectionCandidates.size();++i)
  {
    const int triangleIndex = intersectionCandidates[i];
    const int idx0 = collisionIndices[triangleIndex][0];
    const int idx1 = collisionIndices[triangleIndex][1];
    const int idx2 = collisionIndices[triangleIndex][2];

    const Vec3 &p0 = collisionPositions[idx0];
    const Vec3 &p1 = collisionPositions[idx1];
    const Vec3 &p2 = collisionPositions[idx2];

    if (Intersection::lineTriangle(ray,p0,p1,p2,bary,lambda) &&
      lambda > 0 && lambda < closestLambda)
    {
      closestLambda = lambda;
      closestbary = bary;
      closestTri = triangleIndex;
    }
  }

  if (closestTri >= 0)
  {
    const int i0 = collisionIndices[closestTri][0];
    const int i1 = collisionIndices[closestTri][1];
    const int i2 = collisionIndices[closestTri][2];

    const Vec3 &p0 = collisionPositions[i0];
    const Vec3 &p1 = collisionPositions[i1];
    const Vec3 &p2 = collisionPositions[i2];

    Vec3 n = collisionNormals[i0]*closestbary[0]+
             collisionNormals[i1]*closestbary[1]+
             collisionNormals[i2]*closestbary[2];

    return std::make_shared<RayIntersection>(ray, closestLambda,n);
  }
  return nullptr;
}

std::shared_ptr<RayIntersection>
  CollisionGeometry::closestIntersection(const Ray &ray, float maxLambda) const
{
  //adapt maximal lambda value relative to transformation properties
  //(e.g., scaling)
  Ray  modelRay       = transformRayWorldToModel(ray);
  maxLambda = transformRayLambdaWorldToModel(ray, maxLambda);

  //transform ray from world to model coordinate system
  std::shared_ptr<RayIntersection> isect_local =
    this->closestIntersectionModel(modelRay,maxLambda);

  if (!isect_local)
    return nullptr;

  //transform intersection from model to world coordinate system
  isect_local->transform(mModelMatrix, mModelMatrixInverseTransposed);

  return isect_local;
}

void CollisionGeometry::bind(const GLuint shaderProgram,const GLuint bindingPoint, const std::string &blockName) const
{
  GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, blockName.c_str());
  glUniformBlockBinding(shaderProgram, uniformBlockIndex, bindingPoint);
  glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, mUniformBuffer, 0, sizeof(float)*56);
}

} //namespace ogl