/*
 *  Created on: Apr 04, 2013
 *      Author: martinez
 */

#ifndef VECTOR_HPP_INCLUDE_ONCE
#define VECTOR_HPP_INCLUDE_ONCE

#include <cstddef>
#include <cassert>

#ifdef __GNUC__
#include <initializer_list>
#endif

#include <algorithm>
#include <tuple>
#include <array>
#include <iostream>

#define VECTOR_NO_UNSAFE

namespace util {

using namespace std;

template<typename Tuple, typename FillType, size_t K, size_t offset = 0>
struct TupleFiller {
  static void fill(FillType &v, const Tuple& t) {
    TupleFiller<Tuple, FillType, K-1, offset>::fill(v, t);
    v(K-1+offset) = typename FillType::real(get<K-1>(t));
  }
};

template<typename Tuple, typename FillType, size_t offset>
struct TupleFiller<Tuple, FillType, 1, offset> {
  static void fill(FillType &v, const Tuple& t) {
    v(0+offset) = typename FillType::real(get<0>(t));
  }
};

template<size_t N, typename T>
class Vector {
  static_assert(N>0, "Error: Vector size parameter must be greater than zero!");

  public:
    typedef T real;

    Vector();                                           //! empty constructor (coefficients == 0)
    template<typename TT>
    Vector(const Vector<N,TT> &v);                      //! copy constructor
    template<typename Tuple>
    Vector(const Tuple& t);                             //! tuple value-based constructor

#ifndef VECTOR_NO_UNSAFE
    template<typename TT>
    operator Vector<N,TT>();                            //! type conversion operator
#endif

#ifdef __GNUC__
    template<typename TT>
    Vector(const initializer_list<TT> &l) {             //! initializer_list construct
      assert(l.size() == N);
      copy(begin(l), end(l), begin(mCoefficients));
    }

    template<typename TT>
    Vector<N,T>&
    operator =(const initializer_list<TT> &l) {         //! initializer_list assignment operator
      assert(l.size() == N);
      copy(begin(l), end(l), begin(mCoefficients));
      return *this;
    }

    template<typename ...Args>
    Vector(Args... args) :
      Vector(make_tuple(args ...)) {                    //! varargs constructor
      static_assert(sizeof...(Args) == N, "Error: Wrong number of Vector initialization values!");
    }

    template<size_t NN, typename TT, typename ...Args>
    Vector(const Vector<NN,TT> &v, Args... args) {       //! varargs copy constructor
      static_assert(NN + sizeof...(Args) == N, "Error: Wrong number of Vector initialization values!");

      copy(begin(v.mCoefficients), end(v.mCoefficients), begin(mCoefficients));

      auto tuple = make_tuple(args...);

      TupleFiller<decltype(tuple),Vector<N,T>,N-NN, NN>::fill(*this, tuple);
    }
#else
    Vector(const T& v0, const T& v1);                   //! construct 2-vector
    Vector(const T& v0, const T& v1, const T& v2);      //! construct 3-vector
    Vector(const T& v0, const T& v1, const T& v2,
           const T& v3);                                //! construct 4-vector
    Vector(const T& v0, const T& v1, const T& v2,
           const T& v3, const T& v4);                   //! construct 5-vector
    Vector(const T& v0, const T& v1, const T& v2,
           const T& v3, const T& v4, const T& v5);      //! construct 6-vector

    template<typename TT>
    Vector(const Vector<N-1,TT>& v, const T& v1);       //! vector and scalar constructor
#endif

    Vector<N,T>& operator =(const Vector &v);           //! assignment operator
    template<typename TT>
    Vector& operator =(const Vector<N,TT> &v);          //! assignment operatorMath

    T norm()        const;                              //! Euclidean norm
    T normSquared() const;                              //! squared Euclidean norm
    Vector& normalize();                                //! normalize the vector (only if norm() != 0)
    Vector normalized() const;                          //! normalized vector (only if norm() != 0)

    Vector<N,T> operator -() const;                     //! componentwise negation

    Vector operator +(const T &s) const;                //! scalar multiplication
    Vector operator -(const T &s) const;                //! scalar division
    Vector operator *(const T &s) const;                //! scalar multiplication
    Vector operator /(const T &s) const;                //! scalar division

    Vector operator +(const Vector &v) const;           //! componentwise vector addition
    Vector operator -(const Vector &v) const;           //! componentwise vector substraction
    Vector operator *(const Vector &v) const;           //! componentwise vector multiplication
    Vector operator /(const Vector &v) const;           //! componentwise vector division

    Vector& operator +=(const T &s);                    //! scalar inplace addition
    Vector& operator -=(const T &s);                    //! scalar inplace substraction
    Vector& operator *=(const T &s);                    //! scalar inplace multiplication
    Vector& operator /=(const T &s);                    //! scalar inplace division

    Vector& operator +=(const Vector &v);               //! componentwise vector inplace addition
    Vector& operator -=(const Vector &v);               //! componentwise vector inplace substraction
    Vector& operator *=(const Vector &v);               //! componentwise vector inplace multiplication
    Vector& operator /=(const Vector &v);               //! componentwise vector inplace division

    T&       operator ()(size_t i);                     //! write coefficient
    const T& operator ()(size_t i) const;               //! read coefficient
    T&       operator [](size_t i);                     //! write coefficient
    const T& operator [](size_t i) const;               //! read coefficient

    T dot       (const Vector<N,T> &v) const;           //! dot product
    T operator |(const Vector<N,T> &v) const;           //! dot product

  protected:
    std::array<T,N> mCoefficients;                      //! coefficients

    template<size_t NN, typename TT>
    friend class Vector;
};

template<size_t N, typename T>
Vector<N,T>::Vector() {
  for(size_t i=0; i<N; ++i)
    mCoefficients[i] = T(0);
}

template<size_t N, typename T>
template<typename TT>
Vector<N,T>::Vector(const Vector<N,TT> &v) {
  for(size_t i=0; i<N; ++i)
    mCoefficients[i] = T(v[i]);
}

template<size_t N, typename T>
template<typename Tuple>
Vector<N,T>::Vector(const Tuple& t) {
  static_assert(tuple_size<Tuple>::value == N, "Error: Wrong number of Vector initialization values!");

  TupleFiller<Tuple,Vector<N,T>,N>::fill(*this, t);
}

#ifndef __GNUC__
template<size_t N, typename T>
Vector<N,T>::Vector(const T& v0, const T& v1) {
  *this = make_tuple(v0,v1);
}

template<size_t N, typename T>
Vector<N,T>::Vector(const T& v0, const T& v1, const T& v2) {
  *this = make_tuple(v0,v1,v2);
}

template<size_t N, typename T>
Vector<N,T>::Vector(const T& v0, const T& v1, const T& v2,
                    const T& v3) {
  *this = make_tuple(v0,v1,v2,v3);
}

template<size_t N, typename T>
Vector<N,T>::Vector(const T& v0, const T& v1, const T& v2,
                    const T& v3, const T& v4) {
  *this = make_tuple(v0,v1,v2,v3,v4);
}

template<size_t N, typename T>
Vector<N,T>::Vector(const T& v0, const T& v1, const T& v2,
                    const T& v3, const T& v4, const T& v5) {
  *this = make_tuple(v0,v1,v2,v3,v4,v5);
}

template<size_t N, typename T>
template<typename TT>
Vector<N,T>::Vector(const Vector<N-1,TT> &v, const T& v1) {
  for(size_t i=0; i<N-1; ++i)
    mCoefficients[i] = v[i];
  mCoefficients[N-1] = v1;
}
#endif

#ifndef VECTOR_NO_UNSAFE
template<size_t N, typename T>
template<typename TT>
Vector<N,T>::operator Vector<N,TT>() {
  Vector<N,TT> ret;
  for(size_t i=0; i<N; ++i)
    ret[i] = this->mCoefficients[i];
  return ret;
}
#endif

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator =(const Vector<N,T> &v) {
  if(this == &v)
    return *this;
  for(size_t i=0; i<N; ++i)
    this->mCoefficients[i] = v(i);
  return *this;
}

template<size_t N, typename T>
template<typename TT>
Vector<N,T>& Vector<N,T>::operator =(const Vector<N,TT> &v) {
  for(size_t i=0; i<N; ++i)
    this->mCoefficients[i] = T(v(i));
  return *this;
}

template<size_t N, typename T>
T Vector<N,T>::norm() const {
  return sqrt(normSquared());
}

template<size_t N, typename T>
T Vector<N,T>::normSquared() const {
  return this->dot(*this);
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::normalize() {
  T n = this->norm();
  if(n > T(0))
   return *this /= n;
  return *this;
}
template<size_t N, typename T>
Vector<N,T> Vector<N,T>::normalized() const {
  Vector<N,T> ret = *this;
  T n = ret.norm();
  if(n > T(0))
    return ret /= n;
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator -() const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = -this->mCoefficients[i];
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator +(const T &s) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] + s;
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator -(const T &s) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] - s;
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator *(const T &s) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] * s;
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator /(const T &s) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] / s;
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator +(const Vector& v) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] + v.mCoefficients[i];
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator -(const Vector& v) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] - v.mCoefficients[i];
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator *(const Vector<N,T> &v) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] * v.mCoefficients[i];
  return ret;
}

template<size_t N, typename T>
Vector<N,T> Vector<N,T>::operator /(const Vector<N,T> &v) const {
  Vector<N,T> ret;
  for(size_t i=0; i<N; ++i)
    ret.mCoefficients[i] = this->mCoefficients[i] / v.mCoefficients[i];
  return ret;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator +=(const T &s) {
  for(size_t i=0; i<N; ++i)
    this->mCoefficients[i] += s;
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator -=(const T &s) {
  for(size_t i=0; i<N; ++i)
    this->mCoefficients[i] -= s;
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator *=(const T &s) {
  for(size_t i=0; i<N; ++i)
    this->mCoefficients[i] *= s;
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator /=(const T &s) {
  for(size_t i=0; i<N; ++i)
    this->mCoefficients[i] /= s;
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator +=(const Vector<N,T> &v) {
  for(size_t i=0; i<N; ++i)
    mCoefficients[i] += v.mCoefficients[i];
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator -=(const Vector<N,T> &v) {
  for(size_t i=0; i<N; ++i)
    mCoefficients[i] -= v.mCoefficients[i];
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator *=(const Vector<N,T> &v) {
  for(size_t i=0; i<N; ++i)
    mCoefficients[i] *= v.mCoefficients[i];
  return *this;
}

template<size_t N, typename T>
Vector<N,T>& Vector<N,T>::operator /=(const Vector<N,T> &v) {
  for(size_t i=0; i<N; ++i)
    mCoefficients[i] /= v.mCoefficients[i];
  return *this;
}

template<size_t N, typename T>
T& Vector<N,T>::operator ()(size_t i) {
  assert(i<N);
  return mCoefficients[i];
}

template<size_t N, typename T>
const T& Vector<N,T>::operator ()(size_t i) const {
  assert(i<N);
  return mCoefficients[i];
}

template<size_t N, typename T>
T& Vector<N,T>::operator [](size_t i) {
  assert(i<N);
  return mCoefficients[i];
}

template<size_t N, typename T>
const T& Vector<N,T>::operator [](size_t i) const {
  assert(i<N);
  return mCoefficients[i];
}

template<size_t N, typename T>
T Vector<N,T>::dot(const Vector<N,T> &v) const {
  T ret(0);
  for(size_t i=0; i<N; ++i)
    ret += mCoefficients[i]*v.mCoefficients[i];
  return ret;
}

template<size_t N, typename T>
T Vector<N,T>::operator |(const Vector<N,T> &v) const {
  return this->dot(v);
}

template<size_t N, typename T1, typename T2>
Vector<N,T2> operator *(const T1 &s, const Vector<N,T2> &v) {
  return v * T2(s);
}

template<size_t N, typename T>
inline std::ostream& operator <<(std::ostream& os, const Vector<N,T> &v) {
  os << "[";
  for(size_t i=0; i<N-1; ++i)
    os << v(i) << std::endl << " ";
  os << v(N-1) << "]";
  return os;
}

template<typename T1, typename T2>
Vector<3,decltype(T1(0)*T2(0))> operator %(const Vector<3,T1> &v0,
                                           const Vector<3,T2> &v1) {
  Vector<3,decltype(T1(0)*T2(0))> ret;

  ret(0) = v0(1)*v1(2) - v0(2)*v1(1);
  ret(1) = v0(2)*v1(0) - v0(0)*v1(2);
  ret(2) = v0(0)*v1(1) - v0(1)*v1(0);

  return ret;
}

template<typename T1, typename T2>
Vector<3,decltype(T1(0)*T2(0))> cross(const Vector<3,T1> &v0,
                                      const Vector<3,T2> &v1) {
  return v0 % v1;
}

template<size_t N, typename T1, typename T2>
Vector<N,decltype(T1(0)*T2(0))> reflect(const Vector<N,T1> &v0,
                                        const Vector<N,T2> &v1) {
  return v0 - 2 * (v1 | v0) * v1;
}

typedef Vector<2,float> Vec2f;
typedef Vector<3,float> Vec3f;
typedef Vector<4,float> Vec4f;

typedef Vector<2,int> Vec2i;
typedef Vector<3,int> Vec3i;
typedef Vector<4,int> Vec4i;

} //namespace util

#endif //VECTOR_HPP_INCLUDE_ONCE
