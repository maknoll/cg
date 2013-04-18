#ifndef IMAGE_HPP_INCLUDE_ONCE
#define IMAGE_HPP_INCLUDE_ONCE

#include "Math.hpp"

#include <vector>
#include <string>

namespace rt {

/// Storage for an RGBA floating point image.
class Image
{
public:
  Image();
  Image(size_t width, size_t height);

  /// Valid values for width and height must be > 0.
  void init(size_t width, size_t height);

  /// Writes an integer 4x[0,255] RGBA image in TGA format.
  /// Pixel intensities outside valid range [0,1] are clamped.
  bool saveToTGA(std::string filename)  const;

  size_t width ()                       const { return mWidth; }
  size_t height()                       const { return mHeight; }

  /// Returns the RGBA color at image position i,j
  const Vec4& pixel(size_t i, size_t j) const { return mData[i+mWidth*j]; }

  /// RGBA colors components must be in [0,1] range for TGA export. 
  void setPixel(Vec4 &rgba, size_t i, size_t j) { mData[i+mWidth*j]=rgba; }

private:
  size_t mWidth;
  size_t mHeight;
  std::vector<Vec4> mData;
};

} //namespace rt

#endif //IMAGE_HPP_INCLUDE_ONCE
