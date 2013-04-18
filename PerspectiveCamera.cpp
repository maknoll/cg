#include "PerspectiveCamera.hpp"

namespace rt {

Ray PerspectiveCamera::ray(size_t x, size_t y) const
{
  return Ray(this->position(),
           ((this->topLeft() + x * this->right() - y * this->down()) - this->position()));
}

} //namespace rt
