#ifndef PERSPECTIVECAMERA_HPP_INCLUDE_ONCE
#define PERSPECTIVECAMERA_HPP_INCLUDE_ONCE

#include "Camera.hpp"

namespace rt {

class PerspectiveCamera : public Camera
{
public:
  Ray ray(size_t x, size_t y) const override;
};

} //namespace rt

#endif //PERSPECTIVECAMERA_HPP_INCLUDE_ONCE
