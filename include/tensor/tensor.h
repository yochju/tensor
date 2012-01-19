// -*- mode: c++; fill-column: 80; c-basic-offset: 2; indent-tabs-mode: nil -*-
/*
    Copyright (c) 2010 Juan Jose Garcia Ripoll

    Tensor is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published
    by the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef TENSOR_TENSOR_H
#define TENSOR_TENSOR_H

#include <cassert>
#include <tensor/numbers.h>
#include <tensor/vector.h>
#include <tensor/gen.h>
#include <tensor/indices.h>
#include <tensor/detail/functional.h>

namespace tensor {

//////////////////////////////////////////////////////////////////////
// BASE CLASS
//

/*!\addtogroup Tensors*/
/* @{ */
/**An N-dimensional array of numbers. A Tensor is a multidimensional array of
   numbers. Their behavior is similar to Matlab's arrays in that they can store
   only numbers, be accessed with one or more indices using the () or []
   syntaxes, reshaped, sliced, and all that with an automated memory management.
   \see \ref sec_tensor
*/
template<typename elt>
class Tensor {
 public:
  typedef elt elt_t;
  typedef elt_t *iterator;
  typedef const elt_t *const_iterator;

  /**Constructs an empty Tensor.*/
  Tensor() : data_(), dims_() {}

  /**Constructs an unitialized N-D Tensor given the dimensions.*/
  explicit Tensor(const Indices &new_dims);

  /**Consturcts an N-D Tensor with given initial data.*/
  Tensor(const Indices &new_dims, const Tensor<elt_t> &data);

  /**Optimized copy constructor (See \ref Copy "Optimal copy").*/
  Tensor(const Tensor &other) : data_(other.data_), dims_(other.dims_) {}

  /**Create a one-dimensional tensor from data created with "gen" expressions.*/
  template<size_t n> Tensor(const StaticVector<elt_t,n> &t) :
    data_(t), dims_(igen << t.size())
  {}

  /**Create a general tensor from data created with "gen" expressions.*/
  template<size_t n> Tensor(const StaticVector<elt_t,n> &t, const Indices &d) :
    data_(t), dims_(d)
  {
    assert(data_.size() == d.total_size());
  }

  /**Build a 1D Tensor given the size and the raw C data.*/
  explicit Tensor(index length);
  /**Build a matrix.*/
  Tensor(index rows, index cols);
  /**Build a 3D Tensor.*/
  Tensor(index d1, index d2, index d3);
  /**Build a 4D Tensor.*/
  Tensor(index d1, index d2, index d3, index d4);
  /**Build a 5D Tensor.*/
  Tensor(index d1, index d2, index d3, index d4, index d5);
  /**Build a 6D Tensor.*/
  Tensor(index d1, index d2, index d3, index d4, index d5, index d6);

  /**Returns total number of elements in Tensor.*/
  index size() const { return data_.size(); }
  /**Does the tensor have elements?*/
  bool is_empty() const { return size() == 0; }

  /**Number of Tensor indices.*/
  int rank() const { return dims_.size(); }
  /**Return Tensor dimensions.*/
  const Indices &dimensions() const { return dims_; }
  /**Length of a given Tensor index.*/
  index dimension(int which) const;
  /**Query dimensions of 1D Tensor.*/
  void get_dimensions(index *length) const;
  /**Query dimensions of 2D Tensor.*/
  void get_dimensions(index *rows, index *cols) const;
  /**Query dimensions of 3D Tensor.*/
  void get_dimensions(index *d0, index *d1, index *d2) const;
  /**Query dimensions of 4D Tensor.*/
  void get_dimensions(index *d0, index *d1, index *d2, index *d3) const;
  /**Query dimensions of 5D Tensor.*/
  void get_dimensions(index *d0, index *d1, index *d2, index *d3, index *d4) const;
  /**Query dimensions of 6D Tensor.*/
  void get_dimensions(index *d0, index *d1, index *d2, index *d3, index *d4, index *d5) const;
  /**Query the size of 2nd index.*/
  index columns() const { return dimension(1); }
  /**Query then size of 1st index. */
  index rows() const { return dimension(0); }

  /**Change the dimensions, while keeping the data. */
  void reshape(const Indices &new_dims);

  /**Return the i-th element, accessed in column major order.*/
  const elt_t &operator[](index i) const;
  /**Return an element of a 1D Tensor.*/
  const elt_t &operator()(index i) const;
  /**Return an element of a 2D Tensor.*/
  const elt_t &operator()(index row, index col) const;
  /**Return an element of a 3D Tensor.*/
  const elt_t &operator()(index d0, index d1, index d2) const;
  /**Return an element of a 4D Tensor.*/
  const elt_t &operator()(index d0, index d1, index d2, index d3) const;
  /**Return an element of a 5D Tensor.*/
  const elt_t &operator()(index d0, index d1, index d2, index d3, index d4) const;
  /**Return an element of a 6D Tensor.*/
  const elt_t &operator()(index d0, index d1, index d2, index d3, index d4, index d5w) const;

  /**Return a mutable reference to the i-th element of a Tensor, in column major order.*/
  elt_t &at_seq(index i);
  /**Return a mutable reference to an element of a 1D Tensor.*/
  elt_t &at(index i);
  /**Return a mutable reference to an element of a 2D Tensor.*/
  elt_t &at(index row, index col);
  /**Return a mutable reference to an element of a 3D Tensor.*/
  elt_t &at(index d1, index d2, index d3);
  /**Return a mutable reference to an element of a 4D Tensor.*/
  elt_t &at(index d1, index d2, index d3, index d4);
  /**Return a mutable reference to an element of a 5D Tensor.*/
  elt_t &at(index d1, index d2, index d3, index d4, index d5);
  /**Return a mutable reference to an element of 6D Tensor.*/
  elt_t &at(index d1, index d2, index d3, index d4, index d5, index d6);

  /**Fill with an element.*/
  void fill_with(const elt_t &e);
  /**Fill with zeros.*/
  void fill_with_zeros() { fill_with(number_zero<elt_t>()); }
  /**Fills with random numbers.*/
  void randomize();

  //
  // Tensor slicing
  //
  class view;
  /**Extracts a slice from a 1D Tensor. */
  const view operator()(PRange r) const;
  /**Extracts a slice from a 2D Tensor. */
  const view operator()(PRange r1, PRange r2) const;
  /**Extracts a slice from a 3D Tensor. */
  const view operator()(PRange r1, PRange r2, PRange r3) const;
  /**Extracts a slice from a 4D Tensor. */
  const view operator()(PRange r1, PRange r2, PRange r3, PRange r4) const;
  /**Extracts a slice from a 5D Tensor. */
  const view operator()(PRange r1, PRange r2, PRange r3, PRange r4, PRange r5) const;
  /**Extracts a slice from a 6D Tensor. */
  const view operator()(PRange r1, PRange r2, PRange r3, PRange r4, PRange r5, PRange r6) const;

  class mutable_view;
  /**Mutable slice from a 1D Tensor. */
  mutable_view at(PRange r);
  /**Mutable slice from a 2D Tensor. */
  mutable_view at(PRange r1, PRange r2);
  /**Mutable slice from a 3D Tensor. */
  mutable_view at(PRange r1, PRange r2, PRange r3);
  /**Mutable slice from a 4D Tensor. */
  mutable_view at(PRange r1, PRange r2, PRange r3, PRange r4);
  /**Mutable slice from a 5D Tensor. */
  mutable_view at(PRange r1, PRange r2, PRange r3, PRange r4, PRange r5);
  /**Mutable slice from a 6D Tensor. */
  mutable_view at(PRange r1, PRange r2, PRange r3, PRange r4, PRange r5, PRange r6);

  //
  // Matrix operations
  //
  /**Identity matrix.*/
  static Tensor<elt_t> eye(index rows) { return eye(rows, rows); }
  /**Rectangular identity matrix.*/
  static Tensor<elt_t> eye(index rows, index cols);
  /**Matrix of zeros.*/
  static Tensor<elt_t> zeros(index rows) { return zeros(rows, rows); }
  /**Matrix of ones.*/
  static Tensor<elt_t> zeros(index rows, index cols);
  /**Matrix of ones.*/
  static Tensor<elt_t> ones(index rows) { return ones(rows, rows); }
  /**Matrix of ones.*/
  static Tensor<elt_t> ones(index rows, index cols);

  /**Iterator at the beginning.*/
  iterator begin() { return data_.begin(); }
  /**Iterator at the beginning.*/
  const_iterator begin() const { return data_.begin_const(); }
  /**Iterator at the beginning for const objects.*/
  const_iterator begin_const() const { return data_.begin_const(); }
  /**Iterator at the end for const objects.*/
  const_iterator end_const() const { return data_.end_const(); }
  /**Iterator at the end for const objects.*/
  const_iterator end() const { return data_.end_const(); }
  /**Iterator at the end.*/
  iterator end() { return data_.end(); }

  // Only for testing purposes
  int ref_count() const { return data_.ref_count(); }

 private:
  Vector<elt_t> data_;
  Indices dims_;
};

//////////////////////////////////////////////////////////////////////
// TENSOR GENERIC OPERATIONS
//
//
// RESHAPING
//

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, const Indices &new_dims);

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, index length);

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, index rows, index columns);

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, index d1, index d2, index d3);

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, index d1, index d2, index d3,
		      index d4);

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, index d1, index d2, index d3,
		      index d4, index d5);

/**Return a Tensor with same data and given dimensions.*/
template<typename elt_t>
Tensor<elt_t> reshape(const Tensor<elt_t> &t, index d1, index d2, index d3,
		      index d4, index d5, index d6);

//////////////////////////////////////////////////////////////////////
// ALGEBRA
//
//
// Unary operations
//
template<typename t>
Tensor<t> operator-(const Tensor<t> &);

//
// Binary operations
//
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator+(const Tensor<t1> &a, const Tensor<t2> &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator-(const Tensor<t1> &a, const Tensor<t2> &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator*(const Tensor<t1> &a, const Tensor<t2> &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator/(const Tensor<t1> &a, const Tensor<t2> &b);
template<typename t1, typename t2>
bool operator==(const Tensor<t1> &a, const Tensor<t2> &b);
template<typename t1, typename t2>
bool operator!=(const Tensor<t1> &a, const Tensor<t2> &b);

template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator+(const Tensor<t1> &a, const t2 &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator-(const Tensor<t1> &a, const t2 &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator*(const Tensor<t1> &a, const t2 &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator/(const Tensor<t1> &a, const t2 &b);

template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator+(const t1 &a, const Tensor<t2> &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator-(const t1 &a, const Tensor<t2> &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator*(const t1 &a, const Tensor<t2> &b);
template<typename t1, typename t2>
Tensor<typename Binop<t1,t2>::type> operator/(const t1 &a, const Tensor<t2> &b);

  template<typename elt_t>
  const Tensor<elt_t> kron(const Tensor<elt_t> &a, const Tensor<elt_t> &b);
  template<typename elt_t>
  const Tensor<elt_t> kron2(const Tensor<elt_t> &a, const Tensor<elt_t> &b);

} // namespace tensor

//////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS
//
#ifdef TENSOR_LOAD_IMPL
#include <tensor/detail/tensor_base.hpp>
#include <tensor/detail/tensor_matrix.hpp>
#include <tensor/detail/tensor_kron.hpp>
#endif
#include <tensor/detail/tensor_slice.hpp>
#include <tensor/detail/tensor_reshape.hpp>
#include <tensor/detail/tensor_ops.hpp>

//////////////////////////////////////////////////////////////////////
// EXPLICIT INSTANTIATIONS
//

namespace tensor {

  extern template class Tensor<double>;
  /** Real Tensor with elements of type "double". */
  typedef Tensor<double> RTensor;

  double norm0(const RTensor &r);
  double scprod(const RTensor &a, const RTensor &b);
  double norm2(const RTensor &r);

  RTensor abs(const RTensor &t);
  RTensor cos(const RTensor &t);
  RTensor sin(const RTensor &t);
  RTensor tan(const RTensor &t);
  RTensor cosh(const RTensor &t);
  RTensor sinh(const RTensor &t);
  RTensor tanh(const RTensor &t);
  RTensor exp(const RTensor &t);
  RTensor sqrt(const RTensor &t);

  const RTensor diag(const RTensor &d, int which, int rows, int cols);
  const RTensor diag(const RTensor &d, int which = 0);
  double trace(const RTensor &d);

  const RTensor permute(const RTensor &a, index ndx1 = 0, index ndx2 = -1);
  const RTensor transpose(const RTensor &a);
  inline const RTensor adjoint(const RTensor &a) { return transpose(a); }

  const RTensor fold(const RTensor &a, int ndx1, const RTensor &b, int ndx2);
  const RTensor foldc(const RTensor &a, int ndx1, const RTensor &b, int ndx2);
  const RTensor foldin(const RTensor &a, int ndx1, const RTensor &b, int ndx2);
  const RTensor mmult(const RTensor &a, const RTensor &b);

  void fold_into(RTensor &output, const RTensor &a, int ndx1, const RTensor &b, int ndx2);
  void foldin_into(RTensor &output, const RTensor &a, int ndx1, const RTensor &b, int ndx2);
  void mmult_into(RTensor &output, const RTensor &a, const RTensor &b);

  extern template class Tensor<cdouble>;
  /** Complex Tensor with elements of type "cdouble". */
  typedef Tensor<cdouble> CTensor;

  double norm0(const CTensor &r);
  cdouble scprod(const CTensor &a, const CTensor &b);
  double norm2(const CTensor &r);

  const CTensor to_complex(const RTensor &r);
  inline const CTensor to_complex(const CTensor &r) { return r; }
  const CTensor to_complex(const RTensor &r, const RTensor &i);

  /**Complex conjugate of a real tensor. Returns the same tensor.*/
  inline const RTensor conj(const RTensor &r) { return r; }
  const CTensor conj(const CTensor &c);

  RTensor abs(const CTensor &t);
  CTensor cos(const CTensor &t);
  CTensor sin(const CTensor &t);
  CTensor tan(const CTensor &t);
  CTensor cosh(const CTensor &t);
  CTensor sinh(const CTensor &t);
  CTensor tanh(const CTensor &t);
  CTensor exp(const CTensor &t);
  CTensor sqrt(const CTensor &t);

  const CTensor diag(const CTensor &d, int which, int rows, int cols);
  const CTensor diag(const CTensor &d, int which = 0);
  cdouble trace(const CTensor &d);

  const CTensor permute(const CTensor &a, index ndx1 = 0, index ndx2 = -1);
  const CTensor transpose(const CTensor &a);
  const CTensor adjoint(const CTensor &a);

  const CTensor fold(const CTensor &a, int ndx1, const CTensor &b, int ndx2);
  const CTensor fold(const RTensor &a, int ndx1, const CTensor &b, int ndx2);
  const CTensor fold(const CTensor &a, int ndx1, const RTensor &b, int ndx2);

  const CTensor foldc(const CTensor &a, int ndx1, const CTensor &b, int ndx2);
  const CTensor foldc(const RTensor &a, int ndx1, const CTensor &b, int ndx2);
  const CTensor foldc(const CTensor &a, int ndx1, const RTensor &b, int ndx2);

  const CTensor mmult(const CTensor &a, const CTensor &b);
  const CTensor mmult(const RTensor &a, const CTensor &b);
  const CTensor mmult(const CTensor &a, const RTensor &b);

  const RTensor scale(const RTensor &t, int ndx1, const RTensor &v);
  const CTensor scale(const CTensor &t, int ndx1, const CTensor &v);
  void scale_inplace(RTensor &t, int ndx1, const RTensor &v);
  void scale_inplace(CTensor &t, int ndx1, const CTensor &v);

  const CTensor foldin(const CTensor &a, int ndx1, const CTensor &b, int ndx2);
} // namespace tensor

/* @} */

#endif // !TENSOR_H
