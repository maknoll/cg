#ifndef COLLISION_HPP_INCLUDE_ONCE
#define COLLISION_HPP_INCLUDE_ONCE

#include "OpenGL.hpp"

namespace ogl
{

  class Ray
  {
  public:
    Ray() : mOrigin(0.0,0.0,0.0), mDirection(0.0,0.0,0.0) {}
    Ray(const Vec3 &origin, const Vec3 &direction) :
      mOrigin(origin), mDirection(Vec3(direction).normalize())  {}

    Vec3 pointOnRay(float lambda) const { return mOrigin + mDirection*lambda; }

    const Vec3& origin()    const { return mOrigin; }
    const Vec3& direction() const { return mDirection; }

    void setOrigin(const Vec3& origin)       { mOrigin=origin; }
    void setDirection(const Vec3& direction) { mDirection=Vec3(direction).normalize(); }

    void transform(const Mat4 &transform)
    {
      mOrigin    = transform*mOrigin;
      mDirection = (transform.as3x3()*mDirection).normalize();
    }

    Ray transformed(const Mat4 &transform) const
    {
      return Ray(transform*mOrigin,
        transform.as3x3()*mDirection);
    }

  private:
    Vec3 mOrigin;
    Vec3 mDirection;
  };

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  /// Container class for intersection between a renderable and a ray
  class RayIntersection
  {
  public:

    /// All necessary information for intersection must be passed to
    /// constructor. No mutator methods are available.
    RayIntersection(const Ray &ray,
      const float lambda, const Vec3 &normal) :
    mRay(ray), mLambda(lambda), mNormal(normal)
    {
      mPosition=ray.pointOnRay(mLambda);
    }

    virtual ~RayIntersection() {}

    const Ray& ray()                               const { return mRay; }
    float lambda()                                  const { return mLambda; }
    const Vec3& position()                         const { return mPosition; }
    const Vec3& normal()                           const { return mNormal; }

    virtual void transform(const Mat4 &transform,
      const Mat4 &transformInvTransp)
    {

      //transform local intersection information to global system
      mPosition = transform*mPosition;
      mNormal   = (transformInvTransp.as3x3()*mNormal).normalize();
      mRay.transform(transform);
      mLambda   = (mPosition - mRay.origin()).length();
    }

  protected:
    Ray mRay;
    float mLambda;
    Vec3 mPosition;
    Vec3 mNormal;
  };

/**
 * Represents an axis-aligned bounding box.
 */
class BoundingBox
{
public:
	BoundingBox() : 
    mMin( std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
    mMax(-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity())
  { } 
	BoundingBox(const Vec3 &min, const Vec3 &max) : mMin(min), mMax(max) { }
        
	// Returns true in case of any intersection
	inline bool anyIntersection(const Ray &ray, float maxLambda) const
  {
    //   float tnear = -std::numeric_limits<float>::max(); 
    //   float tfar = std::numeric_limits<float>::max();
    float tnear = 0; 
    float tfar = maxLambda;
    float t1;
    float t2;

    Vec3 direction = ray.direction();
    Vec3 origin = ray.origin();

    for (int i=0; i<3; i++)
    {
      if (direction[i]) // this is numerically unstable for small values!
      {
        t1 = (mMin[i]-origin[i])/direction[i];
        t2 = (mMax[i]-origin[i])/direction[i];
        if (t1 > t2){
          float tmp = t2;
          t2 = t1;
          t1 = tmp;
        }
        if (t1 > tnear)
          tnear = t1;
        if (t2 < tfar)
          tfar = t2;
        if (tnear > tfar)
          return false;
        if (tfar < 0)
          return false;
      }else{
        if (origin[i]<mMin[i] || origin[i]>mMax[i]) //is this right? we might not reject some trivial cases
          return false;
      }
    }

    return true;
  }

  void merge(const BoundingBox& bb) // merge with a given bounding box
  {
    mMin = Vec3(std::min(bb.min()[0],mMin[0]),std::min(bb.min()[1],mMin[1]),std::min(bb.min()[2],mMin[2]));
    mMax = Vec3(std::max(bb.max()[0],mMax[0]),std::max(bb.max()[1],mMax[1]),std::max(bb.max()[2],mMax[2]));
  }      

	const Vec3& min() const { return mMin; }
	const Vec3& max() const { return mMax; }

  void setMin(const Vec3 &min) { mMin = min; }
  void setMax(const Vec3 &max) { mMax = max; }

	void expandByPoint(const Vec3& p)
  {
    for (int i=0;i<3;++i)
    {
      mMin[i]=std::min(mMin[i],p[i]);
      mMax[i]=std::max(mMax[i],p[i]);
    }
  }

  float computeArea() const
  {
    Vec3 d = mMax-mMin;
    return 2*(d[0]*d[1]+d[0]*d[2]+d[1]*d[2]);
  }

private:
    Vec3 mMin; // Coordinate of min corner
    Vec3 mMax; // Coordinate of max corner
};

} //namespace ogl

#endif //COLLISION_HPP_INCLUDE_ONCE
