#ifndef BOUNDINGBOX_HPP_INCLUDE_ONCE
#define BOUNDINGBOX_HPP_INCLUDE_ONCE

#include "Math.hpp"
#include "Ray.hpp"

namespace rt
{

/**
 * Represents an axis-aligned bounding box.
 */
class BoundingBox
{
public:
	BoundingBox() : 
    mMin( std::numeric_limits<real>::infinity(), std::numeric_limits<real>::infinity(), std::numeric_limits<real>::infinity()),
    mMax(-std::numeric_limits<real>::infinity(),-std::numeric_limits<real>::infinity(),-std::numeric_limits<real>::infinity())
  { } 
	BoundingBox(const Vec3 &min, const Vec3 &max) : mMin(min), mMax(max) { }
        
	// Returns true in case of any intersection
	bool anyIntersection(const Ray &ray, real maxLambda) const;

  void merge(const BoundingBox& box); // merge with a given bounding box
        
	const Vec3& min() const { return mMin; }
	const Vec3& max() const { return mMax; }

  void setMin(const Vec3 &min) { mMin = min; }
  void setMax(const Vec3 &max) { mMax = max; }

	void expandByPoint(const Vec3& point);

  real computeArea() const;

private:
    Vec3 mMin; // Coordinate of min corner
    Vec3 mMax; // Coordinate of max corner
};

}

#endif

