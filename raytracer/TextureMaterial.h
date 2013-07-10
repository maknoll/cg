//
//  TextureMaterial.h
//  cg
//
//  Created by Martin Knoll on 04.07.13.
//  Copyright (c) 2013 Martin Knoll. All rights reserved.
//

#ifndef __cg__TextureMaterial__
#define __cg__TextureMaterial__


#include "Material.hpp"
#include "Image.hpp"

namespace rt
{
  
  class RayIntersection;
  class Light;
  
  class TextureMaterial : public Material
  {
  public:
	TextureMaterial(std::shared_ptr<Image> texture,
				  real reflectance=1.0,
				  real shininess=10.0);
	
	Vec4 shade(std::shared_ptr<RayIntersection> intersection,
			   const Light& light) const override;
	
	real shininess() const { return mShininess; }
	
  private:
	
	real mShininess;
  protected:
	std::shared_ptr<Image> mTexture;
  };
  
}


#endif /* defined(__cg__TextureMaterial__) */
