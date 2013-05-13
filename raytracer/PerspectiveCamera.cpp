#include "PerspectiveCamera.hpp"

namespace rt {

Ray PerspectiveCamera::ray(size_t x, size_t y) const
{
  return Ray(this->position(),
           ((this->topLeft() + this->right()*real(x) - this->down()*real(y)) - this->position()));
}

} //namespace rt
