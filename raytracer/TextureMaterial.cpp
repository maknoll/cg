//
//  TextureMaterial.cpp
//  cg
//
//  Created by Martin Knoll on 04.07.13.
//  Copyright (c) 2013 Martin Knoll. All rights reserved.
//

#include "TextureMaterial.h"
#include "Light.hpp"
#include "Math.hpp"
#include "Ray.hpp"

namespace rt
{
  
  TextureMaterial::TextureMaterial(std::shared_ptr<Image> texture, real reflectance, real shininess) :
  Material(Vec3(1,1,1),reflectance), mShininess(shininess), mTexture(texture)
  {
	
  }
  Vec4 TextureMaterial::shade(std::shared_ptr<RayIntersection> intersection,
							const Light& light) const
  {
	// get normal and light direction
	Vec3 N = intersection->normal();
	Vec3 L = (light.position() - intersection->position()).normalized();
	
	real cosNL = std::max(N.dot(L),real(0));
	
	Vec3 V = intersection->ray().direction();
	Vec3 R = util::reflect(L, N);
	
	real cosRV = std::max(R.dot(V),real(0));
	
	// Programming TASK 3: Extend this method.
	// This method currently implements a Lambert's material with ideal
	// diffuse reflection.
	// Your task is to implement a Phong, or a Blinn-Phong shading model.
	const Vec3& texcoord = intersection->uvw();
	Vec4 color = this->mTexture->getTexPixel(texcoord[0], texcoord[1]);
	Vec3 lightcolor = light.spectralIntensity() / 255;
	Vec3 diffuse = Vec3(color[0],color[1],color[2])*cosNL;
	Vec3 specular = reflectance() * ((shininess() + 2) / (2 * M_PI)) * pow(cosRV,shininess()) * lightcolor;
	return Vec4(diffuse + specular, 1);
  }
  
}