/*
 *  Created on: Apr 06, 2013
 *      Author: martinez
 */

#ifndef MATRIX_HPP_INCLUDE_ONCE
#define MATRIX_HPP_INCLUDE_ONCE

#include "Vector.hpp"

namespace util {

using namespace std;

template<size_t M, size_t N, typename T>
class Matrix {
  static_assert(M>0 && N>0, "Error: Matriz size parameters must be greater than zero!");

  public:
    typedef T           real;
    typedef Vector<N,T> row_type;
    typedef Vector<M,T> column_type;

    Matrix();                                                         //! empty constructor (matrix == identity)
    template<typename TT>
    Matrix(const Matrix<M,N,TT> &m);                                  //! copy constructor
    template<typename Tuple>
    Matrix(const Tuple& t);                                           //! tuple value-based constructor

#ifdef __GNUC__
    template<typename TT>
    Matrix(const initializer_list<TT> &l) {                           //! initializer_list construct
      assert(l.size() == M*N);
      copy(begin(l), end(l), begin(mCoefficients));
    }

    template<typename TT>
    Matrix<M,N,T>&
    operator =(const initializer_list<TT> &l) {                        //! initializer_list assignment operator
      assert(l.size() == M*N);
      copy(begin(l), end(l), begin(mCoefficients));
      return *this;
    }

    template<typename ...Args>
    Matrix(Args... args) :
      Matrix(make_tuple(args ...)) {                                  //! varargs constructor
      static_assert(sizeof...(Args) == M*N,
                    "Error: Wrong number of Vector initialization values!");
    }
#endif

    Matrix& operator =(const Matrix &m);                              //! assignment operator
    template<typename TT>
    Matrix& operator =(const Matrix<M,N,TT> &m);                      //! assignment operator
    template<typename Tuple>
    Matrix& operator =(const Tuple& t);                               //! tuple value-based assignment operator

    size_t nrows() const { return M; }                                //! number of rows
    size_t ncols() const { return N; }                                //! number of columns

    Matrix& setIdentity();                                            //! reset to identity

    Matrix&       transpose();                                        //! inplace transpose matrix
    Matrix<N,M,T> transposed() const;                                 //! transposed matrix

    bool invert();                                                    //! inplace matrix inversion (true if successful)

    T&       operator ()(size_t i, size_t j);                         //! write coefficient
    const T& operator ()(size_t i, size_t j) const;                   //! read coefficient
    T&       operator ()(size_t k);                                   //! write coefficient (linear indexing)
    const T& operator ()(size_t k) const;                             //! read coefficient  (linear indexing)

    const row_type&      row(size_t i) const;                         //! row access
    const column_type column(size_t j) const;                         //! column access

#ifndef _MSC_VER
    template<size_t MM, size_t NN, size_t i=0, size_t j=0>
#else
  template<size_t MM, size_t NN, size_t i, size_t j>
#endif
    Matrix<MM,NN,T> submatrix() const;                                //! submatrix access

    template<typename TT>
    Matrix& operator *=(const Matrix<M,N,TT> &m);                     //! matrix inplace multiplication

    template<typename TT>
    Matrix& multLeft(const Matrix<M,N,TT> &m);                        //! matrix inplace left multiplication

    template<typename TT>
    Vector<M,decltype(T(0)*TT(0))>                                    //! matrix vector multiplication
    operator *(const Vector<N,TT> &v) const;

  protected:
    std::array<row_type,M> mCoefficients;                             //! coefficients (row-wise vectors)
};

template<size_t M, size_t N, typename T>
Matrix<M,N,T>::Matrix() {
  size_t k = 0;
  while(k < M && k < N) {
    mCoefficients[k][k] = T(1);
    ++k;
  }
}

template<size_t M, size_t N, typename T>
template<typename TT>
Matrix<M,N,T>::Matrix(const Matrix<M,N,TT> &m) {
  for(size_t i=0; i<M; ++i)
    for(size_t j=0; j<N; ++j)
        mCoefficients[i][j] = T(m(i,j));
}

template<size_t M, size_t N, typename T>
template<typename Tuple>
Matrix<M,N,T>::Matrix(const Tuple& t) {
  static_assert(tuple_size<Tuple>::value == M*N, "Error: Wrong number of Matrix initialization values!");

  TupleFiller<Tuple,Matrix<M,N,T>,M*N>::fill(*this, t);
}

template<size_t M, size_t N, typename T>
Matrix<M,N,T>& Matrix<M,N,T>::operator =(const Matrix<M,N,T> &m) {
  if(this == &m)
    return *this;
  for(size_t i=0; i<M; ++i)
    for(size_t j=0; j<N; ++j)
        mCoefficients[i][j] = m(i,j);
  return *this;
}

template<size_t M, size_t N, typename T>
template<typename TT>
Matrix<M,N,T>& Matrix<M,N,T>::operator =(const Matrix<M,N,TT> &m) {
  for(size_t i=0; i<M; ++i)
    for(size_t j=0; j<N; ++j)
        mCoefficients[i][j] = T(m(i,j));
  return *this;
}

template<size_t M, size_t N, typename T>
template<typename Tuple>
Matrix<M,N,T>& Matrix<M,N,T>::operator =(const Tuple& t) {
  static_assert(tuple_size<Tuple>::value == M*N, "Error: Wrong number of Matrix initialization values!");

  TupleFiller<Tuple,Matrix<M,N,T>,M*N>::fill(*this, t);

  return *this;
}

template<size_t M, size_t N, typename T>
Matrix<M,N,T>& Matrix<M,N,T>::setIdentity() {
  for(size_t i=0; i<M; ++i)
    for(size_t j=0; j<N; ++j)
      if(i==j)
        mCoefficients[i][j] = T(1);
      else
        mCoefficients[i][j] = T(0);

  return *this;
}

template<size_t M, size_t N, typename T>
Matrix<M,N,T>& Matrix<M,N,T>::transpose() {
  static_assert(M==N, "Error: Require square matrix for inplace transpose!");

  column_type new_rows[M];
  for(size_t j=0; j<N; ++j)
    new_rows[j] = column(j);

  for(size_t i=0; i<M; ++i)
    mCoefficients[i] = new_rows[i];

  return *this;
}

template<size_t M, size_t N, typename T>
Matrix<N,M,T> Matrix<M,N,T>::transposed() const {
  Matrix<N,M,T> ret;

  for(size_t i=0; i<M; ++i)
    for(size_t j=0; j<N; ++j)
      ret(j,i) = mCoefficients[i][j];

  return ret;
}

template<size_t M, size_t N, typename T>
bool Matrix<M,N,T>::invert() {
  static_assert(M==N, "Error: Require square matrix for inversion!");

  static_assert(M==4, "Error: Only 4-matrix inversion supported at the moment!");

  const real a = (*this)(0);  const real b = (*this)(1);  const real c = (*this)(2);  const real d = (*this)(3);
  const real e = (*this)(4);  const real f = (*this)(5);  const real g = (*this)(6);  const real h = (*this)(7);
  const real i = (*this)(8);  const real l = (*this)(9);  const real m = (*this)(10); const real n = (*this)(11);
  const real o = (*this)(12); const real p = (*this)(13); const real q = (*this)(14); const real r = (*this)(15);

  const real mr = m*r;
  const real gn = g*n;
  const real el = e*l;
  const real ip = i*p;
  const real mo = m*o;
  const real hl = h*l;
  const real mp = m*p;
  const real nq = n*q;
  const real gl = g*l;
  const real no = n*o;
  const real gi = g*i;
  const real np = n*p;
  const real fi = f*i;
  const real rc = r*c;
  const real be = b*e;
  const real af = a*f;
  const real de = d*e;
  const real df = d*f;
  const real ch = c*h;
  const real qh = q*h;

  real Ca = +(( f*mr + gn*p + hl*q ) - ( h*mp + nq*f + r*gl ));
  real Cb = -(( e*mr + gn*o + i*qh ) - ( h*mo + gi*r + nq*e ));
  real Cc = +(( el*r + ip*h + f*no ) - ( hl*o + np*e + fi*r ));
  real Cd = -(( el*q + f*mo + g*ip ) - ( gl*o + mp*e + q*fi ));

  real det = a*Ca + b*Cb + c*Cc + d*Cd;

  // singular matrix
  if (!(det > real(0)))
    return false;

  real Ce = -(( b*mr + c*np + d*l*q ) - ( d*mp + nq*b + rc*l ));
  real Cf = +(( a*mr + c*no + d*i*q ) - ( d*mo + nq*a + rc*i ));
  real Cg = -(( a*l*r + b*no + d*ip ) - ( d*l*o + np*a + r*b*i ));
  real Ch = +(( a*l*q + b*mo + c*ip ) - ( c*l*o + mp*a + q*b*i ));
  real Ci = +(( b*g*r + ch*p + df*q ) - ( d*g*p + q*h*b + rc*f ));
  real Cl = -(( a*g*r + ch*o + de*q ) - ( d*g*o  + qh*a + rc*e ));
  real Cm = +(( af*r + b*h*o + de*p ) - ( df*o + h*p*a + r*be ));
  real Cn = -(( af*q + b*g*o + c*e*p ) - ( c*f*o + g*p*a + q*be ));
  real Co = -(( b*gn + c*hl + df*m ) - ( d*gl + h*m*b + n*c*f ));
  real Cp = +(( a*gn + ch*i + de*m ) - ( d*gi + h*m*a + n*c*e ));
  real Cq = -(( af*n + b*h*i + d*el ) - ( d*fi + hl*a + n*be ));
  real Cr = +(( af*m + b*gi + c*el ) - ( c*fi + gl*a + m*be ));

  real inv_det = 1 / det;

  (*this)(0,0) = inv_det * Ca;
  (*this)(1,0) = inv_det * Cb;
  (*this)(2,0) = inv_det * Cc;
  (*this)(3,0) = inv_det * Cd;
  (*this)(0,1) = inv_det * Ce;
  (*this)(1,1) = inv_det * Cf;
  (*this)(2,1) = inv_det * Cg;
  (*this)(3,1) = inv_det * Ch;
  (*this)(0,2) = inv_det * Ci;
  (*this)(1,2) = inv_det * Cl;
  (*this)(2,2) = inv_det * Cm;
  (*this)(3,2) = inv_det * Cn;
  (*this)(0,3) = inv_det * Co;
  (*this)(1,3) = inv_det * Cp;
  (*this)(2,3) = inv_det * Cq;
  (*this)(3,3) = inv_det * Cr;

  return true;
}

template<size_t M, size_t N, typename T>
T& Matrix<M,N,T>::operator ()(size_t i, size_t j) {
  assert(i<M && j<N);
  return mCoefficients[i][j];
}

template<size_t M, size_t N, typename T>
const T& Matrix<M,N,T>::operator ()(size_t i, size_t j) const {
  assert(i<M && j<N);
  return mCoefficients[i][j];
}

template<size_t M, size_t N, typename T>
T& Matrix<M,N,T>::operator ()(size_t k) {
  assert(k<M*N);
  return mCoefficients[k/N][k%N];
}

template<size_t M, size_t N, typename T>
const T& Matrix<M,N,T>::operator ()(size_t k) const {
  assert(k<M*N);
  return mCoefficients[k/N][k%N];
}

template<size_t M, size_t N, typename T>
const typename Matrix<M,N,T>::row_type& Matrix<M,N,T>::row(size_t i) const {
  assert(i<M);
  return mCoefficients[i];
}

template<size_t M, size_t N, typename T>
const typename Matrix<M,N,T>::column_type Matrix<M,N,T>::column(size_t j) const {
  assert(j<N);

  column_type ret;
  for(size_t i=0; i<M; ++i)
    ret[i] = mCoefficients[i][j];

  return ret;
}

template<size_t M, size_t N, typename T>
template<size_t MM, size_t NN, size_t i, size_t j>
Matrix<MM,NN,T> Matrix<M,N,T>::submatrix() const {
  static_assert(i+MM<=M && j+NN<=N, "Error: Dimensions incompatible!");

  Matrix<MM,NN,T> ret;

  for(size_t ii=0; ii<MM; ++ii)
    for(size_t jj=0; jj<NN; ++jj)
      ret(ii,jj) = mCoefficients[i+ii][j+jj];

  return ret;
}

template<size_t M, size_t N, typename T>
template<typename TT>
Matrix<M,N,T>& Matrix<M,N,T>::operator *=(const Matrix<M,N,TT> &m) {

  column_type rhs_cols[N];
  for(size_t j=0; j<N; ++j)
    rhs_cols[j] = m.column(j);

  row_type new_row;
  for(size_t i=0; i<M; ++i) {
    for(size_t j=0; j<N; ++j)
      new_row[j] = row(i) | rhs_cols[j];
    mCoefficients[i] = new_row;
  }

  return *this;
}

template<size_t M, size_t N, typename T>
template<typename TT>
Matrix<M,N,T>& Matrix<M,N,T>::multLeft(const Matrix<M,N,TT> &m) {

  column_type rhs_cols[N];
  for(size_t j=0; j<N; ++j)
    rhs_cols[j] = column(j);

  for(size_t i=0; i<M; ++i)
    for(size_t j=0; j<N; ++j)
      mCoefficients[i][j] = m.row(i) | rhs_cols[j];

  return *this;
}

template<size_t M, size_t N, typename T>
template<typename TT>
Vector<M,decltype(T(0)*TT(0))>
Matrix<M,N,T>::operator *(const Vector<N,TT> &v) const {
  Vector<M,decltype(T(0)*TT(0))> ret;

  for(size_t i=0; i<M; ++i)
    ret(i) = row(i) | v;

  return ret;
}

//! matrix matrix multiplication
template<size_t M1, size_t N1,
         size_t M2, size_t N2,
         typename T1, typename T2>
Matrix<M1,N2,decltype(T1(0)*T2(0))> operator *(const Matrix<M1,N1,T1> &m1,
                                               const Matrix<M2,N2,T2> &m2) {
  static_assert(N1==M1, "Error: Matrix dimensions missmatch!");

  typedef Matrix<M1,N2,decltype(T1(0)*T2(0))> RetType;

  RetType ret;

  typename RetType::column_type rhs_cols[N2];
  for(size_t j=0; j<N2; ++j)
    rhs_cols[j] = m2.column(j);

  for(size_t i=0; i<M1; ++i) {
    for(size_t j=0; j<N2; ++j)
      ret(i,j) = m1.row(i) | rhs_cols[j];
  }

  return ret;
}

template<size_t M, size_t N, typename T>
inline std::ostream& operator <<(std::ostream& os, const Matrix<M,N,T> &m) {
  os << "[";
  for(size_t i=0; i<M; ++i) {
    for(size_t j=0; j<N; ++j) {
      os << m(i,j);
      if(j!=N-1)
        os << " ";
    }
    if(i==M-1)
      os << "]";
    else
      os << std::endl << " ";
  }
  return os;
}

template<size_t M, typename T>
class AffineMatrix : public Matrix<M,M,T> {
public:
  typedef typename Matrix<M,M,T>::real real;

  AffineMatrix() : Matrix<M,M,T>() {}                                 //! empty constructor (matrix == identity)
  template<typename TT>
  AffineMatrix(const Matrix<M,M,TT> &m) : Matrix<M,M,T>(m) {}         //! copy constructor
  template<typename Tuple>
  AffineMatrix(const Tuple& t) : Matrix<M,M,T>(t) {}                  //! tuple value-based constructor

#ifdef __GNUC__
    template<typename TT>
    AffineMatrix(const initializer_list<TT> &l)
      : Matrix<M,M,T>(l) {}                                           //! initializer_list construct

    template<typename ...Args>
    AffineMatrix(Args... args) : Matrix<M,M,T>(args ...) {}           //! varargs constructor
#endif

  template<typename TT>
  Vector<M-1,decltype(T(0)*TT(0))>                                    //! transform a M-1 vector with the upper left matrix part
  transformVector(const Vector<M-1,TT> &v) const;

  template<typename TT>
  Vector<M-1,decltype(T(0)*TT(0))>                                    //! transform a homogeneous M-1 point with the upper matrix part
  transformPoint(const Vector<M-1,TT> &v) const;

  template<typename TT>
  AffineMatrix&                                                       //! translate transformation
  translate(const Vector<M-1,TT> &v);

  template<typename TT>
  AffineMatrix&                                                       //! scale transformation
  scale(const Vector<M-1,TT> &s);

  template<typename TT>
  AffineMatrix&                                                       //! scale transformation uniformly
  scale(const TT &s);

  template<typename TT, typename TTT>
  AffineMatrix&                                                       //! axis / angle 3D rotation of 3/4 matrices
  rotate(const Vector<3,TT> &axis, const TTT &angle);
};

template<size_t M, typename T>
template<typename TT>
Vector<M-1,decltype(T(0)*TT(0))>
AffineMatrix<M,T>::transformVector(const Vector<M-1,TT> &v) const {
  return this->template submatrix<M-1,M-1,0,0>() * v;
}

template<size_t M, typename T>
template<typename TT>
Vector<M-1,decltype(T(0)*TT(0))>
AffineMatrix<M,T>::transformPoint(const Vector<M-1,TT> &v) const {
  typedef Vector<M-1,decltype(T(0)*TT(0))> RetType;

  Vector<M,TT> vv;
  for(size_t i=0; i<M-1; ++i)
    vv(i) = v(i);
  vv(M-1) = T(1);

  vv = this->operator*(vv);

  auto w = vv(M-1);

  RetType ret;

  if(!(fabs(w) > typename AffineMatrix<M,T>::real(0)))
    return ret;

  for(size_t i=0; i<M-1; ++i)
    ret(i) = vv(i) / w;

  return ret;
}

template<size_t M, typename T>
template<typename TT>
AffineMatrix<M,T>& AffineMatrix<M,T>::translate(const Vector<M-1,TT> &v) {
  AffineMatrix<M,T> Transf;

  for(size_t i=0; i<M-1; ++i)
    Transf(i,M-1) = v(i);

  this->multLeft(Transf);

  return *this;
}

template<size_t M, typename T>
template<typename TT>
AffineMatrix<M,T>& AffineMatrix<M,T>::scale(const Vector<M-1,TT> &s) {
  AffineMatrix<M,T> Transf;

  for(size_t i=0; i<M-1; ++i)
    Transf(i,i) = T(s(i));

  this->multLeft(Transf);

  return *this;
}

template<size_t M, typename T>
template<typename TT>
AffineMatrix<M,T>& AffineMatrix<M,T>::scale(const TT &s) {
  Vector<M-1,TT> ss;
  for(size_t i=0; i<M-1; ++i)
    ss(i) = s;

  return this->scale(ss);
}

template<size_t M, typename T>
template<typename TT, typename TTT>
AffineMatrix<M,T>& AffineMatrix<M,T>::rotate(const Vector<3,TT> &axis,
                                             const TTT &angle) {
  static_assert(M==3 || M==4, "Error: Only 3D AffineMatrix matrix rotation supported at the moment");

  Vector<3,T> a(axis); a.normalize();
  const T &a1 = a(0), &a2 = a(1), &a3 = a(2);

  const T s = sin(angle), cm1 = cos(angle) - T(1);

  AffineMatrix<4,T> R;

  R(0,0) = 1 - (a1*a1 - 1)*cm1; R(0,1) = - a3*s - a1*a2*cm1;  R(0,2) = a2*s - a1*a3*cm1;
  R(1,0) = a3*s - a1*a2*cm1;    R(1,1) = 1 - (a2*a2 - 1)*cm1; R(1,2) = - a1*s - a2*a3*cm1;
  R(2,0) = - a2*s - a1*a3*cm1;  R(2,1) = a1*s - a2*a3*cm1;    R(2,2) = 1 - (a3*a3 - 1)*cm1;

  this->multLeft(R);

  return *this;
}

typedef AffineMatrix<2,float> Mat2f;
typedef AffineMatrix<3,float> Mat3f;
typedef AffineMatrix<4,float> Mat4f;

typedef AffineMatrix<2,double> Mat2d;
typedef AffineMatrix<3,double> Mat3d;
typedef AffineMatrix<4,double> Mat4d;

typedef AffineMatrix<2,int> Mat2i;
typedef AffineMatrix<3,int> Mat3i;
typedef AffineMatrix<4,int> Mat4i;

} //namespace util

#endif // MATRIX_HPP_INCLUDE_ONCE
