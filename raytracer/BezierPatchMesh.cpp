#include "BezierPatchMesh.hpp"

namespace rt
{

BezierPatchMesh::BezierPatchMesh(size_t m,    size_t n,
                                 size_t resu, size_t resv) :
  mM(m), mN(n), TriangleMesh(), mResU(resu), mResV(resv)
{
  // Allocate memory for Bezier points
  mControlPoints.resize(m*n);
}

void BezierPatchMesh::initialize()
{
  //this function samples the underlying continuous patch and tessellates it
  //regularly with triangles

  //sample at triangle vertices at uniform uv parameters
  std::vector<BezierPatchSample> samples; samples.reserve(mResU*mResV);

  for(size_t j=0; j<mResV; ++j)
    for(size_t i=0; i<mResU; ++i)
      samples.push_back(this->sample(real(i) / (mResU-1), real(j) / (mResV-1)));

  //construct triangle strips based on the computed samples
  for(size_t j=0; j<mResV-1; ++j)
  {
    for(size_t i=0; i<mResU-1; ++i)
    {
      const size_t i00 = mResU* j    +  i,
                   i10 = mResU* j    + (i+1),
                   i01 = mResU*(j+1) +  i,
                   i11 = mResU*(j+1) + (i+1);

      {
        //construct lower triangle
        const Vec3 &v0 = samples[i00].position,
          &v1 = samples[i10].position,
          &v2 = samples[i01].position;
        const Vec2 &p0 = samples[i00].uv,
          &p1 = samples[i10].uv,
          &p2 = samples[i01].uv;
        Vec3  n0 = samples[i00].normal,
              n1 = samples[i10].normal,
              n2 = samples[i01].normal;

        if(!n0.normSquared() || !n1.normSquared() || !n2.normSquared())
        {
          //fall back to triangle normals if normals are not defined
          const Vec3 normal = ((v1-v0) % (v2-v0)).normalize();
          n0 = normal; n1 = normal; n2 = normal;
        }
        this->addTriangle(v0,v1,v2,n0,n1,n2,Vec3(p0,real(0)),Vec3(p1,real(0)),Vec3(p2,real(0)));
      }

      {
        //construct upper triangle
        const Vec3 &v0 = samples[i10].position,
          &v1 = samples[i11].position,
          &v2 = samples[i01].position;
        const Vec2 &p0 = samples[i10].uv,
          &p1 = samples[i11].uv,
          &p2 = samples[i01].uv;
        Vec3 n0 = samples[i10].normal,
             n1 = samples[i11].normal,
             n2 = samples[i01].normal;

        if(!n0.normSquared() || !n1.normSquared() || !n2.normSquared())
        {
          //fall back to triangle normals if normals are not defined
          const Vec3 normal = ((v1-v0) % (v2-v0)).normalize();
          n0 = normal; n1 = normal; n2 = normal;
        }
        this->addTriangle(v0,v1,v2,n0,n1,n2,Vec3(p0,real(0)),Vec3(p1,real(0)),Vec3(p2,real(0)));
      }
    }
  }
}
BezierPatchMesh::BezierPatchSample BezierPatchMesh::sample(real u, real v) const
{
  BezierPatchSample ret;
  ret.uv = Vec2(u,v);

  // Programming TASK 2: implement this method
  // You need to compute ret.position and ret.normal!

  // This data will be used within the initialize() function for the 
  // triangle mesh construction.

  ret.position = Vec3();
  ret.normal = Vec3();
  return ret;
}

} //namespace rt
