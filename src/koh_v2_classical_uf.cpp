
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <span>

#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "DisjointSet.h"

namespace py = pybind11;

// helper function to avoid making a copy when returning a py::array_t
// author: https://github.com/YannickJadoul
// source: https://github.com/pybind/pybind11/issues/1042#issuecomment-642215028
template <typename Sequence>
inline py::array_t<typename Sequence::value_type> as_pyarray(Sequence &&seq) {
   auto size = seq.size();
   auto data = seq.data();
   std::unique_ptr<Sequence> seq_ptr = std::make_unique<Sequence>(std::move(seq));
   auto capsule = py::capsule(seq_ptr.get(), 
                              [](void *p) 
                              {
                                 std::unique_ptr<Sequence>(reinterpret_cast<Sequence *>(p));
                              }
                              );
   seq_ptr.release();

   return py::array(size, data, capsule);
}

/**
 * \brief Returns span<T> from py:array_T<T>. Efficient as zero-copy.
 * \tparam T Type.
 * \param passthrough Numpy array.
 * \return Span<T> that with a clean and safe reference to contents of Numpy array.
 */
template<typename T>
inline std::span<T> toSpan(py::array_t<T, py::array::f_style> const & passthrough)
{
	py::buffer_info passthroughBuf = passthrough.request();
	if (passthroughBuf.ndim != 2) {
		throw std::runtime_error("Error. Number of dimensions must be two");
	}
	size_t length = passthroughBuf.shape[0] * passthroughBuf.shape[1];
	T* passthroughPtr = static_cast<T*>(passthroughBuf.ptr);
	std::span<T> passthroughSpan(passthroughPtr, length);
	return passthroughSpan;
}

template<typename T>
inline std::span<T> toSpan1D(py::array_t<T, py::array::c_style> const & passthrough)
{
	py::buffer_info passthroughBuf = passthrough.request();
	if (passthroughBuf.ndim != 1) {
		throw std::runtime_error("Error. Number of dimensions must be two");
	}
	size_t length = passthroughBuf.shape[0];
	T* passthroughPtr = static_cast<T*>(passthroughBuf.ptr);
	std::span<T> passthroughSpan(passthroughPtr, length);
	return passthroughSpan;
}

template <typename T>
static void sort_indexes(py::array_t<T> const &v, 
                           std::span<size_t> & idx) 
{

  // initialize original index locations
   //std::vector<size_t> idx(v.size());
   //std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values 
   std::stable_sort(idx.begin(), idx.end(), 
                     [&v](size_t i1, size_t i2) 
                     {
                        return std::less<T>{}(v.data()[i1], v.data()[i2]);//v.data()[i1] < v.data()[i2];
                     }
                  );

   //return idx;
}

py::array_t<size_t> koh_v2_classical_uf(py::array_t<ssize_t, py::array::f_style> const & hog, 
                                          py::array_t<float> const & llrs, 
                                          size_t const & orig_hog_rows,
                                          py::array_t<size_t, py::array::c_style> idxs,
                                          size_t const & rank)
{
   const size_t hog_rows = hog.shape(0);
   const size_t hog_cols = hog.shape(1);
   
   std::vector<size_t> columns_chosen;
   columns_chosen.reserve(hog_cols);

   std::span<size_t> idxs_sp = toSpan1D(idxs);
   sort_indexes(llrs, idxs_sp);
   std::span<ssize_t> hog_sp = toSpan(hog);

   DisjSet clstr_set = DisjSet(orig_hog_rows);

   for (size_t col_idx = 0UL; col_idx < idxs_sp.size(); col_idx++)
   {
      size_t effective_col_idx = idxs_sp.data()[col_idx];
      size_t col_offset = effective_col_idx * hog_rows;
      std::span<ssize_t> column_sp(hog_sp.data() + col_offset, hog_rows);

      long int retcode = -1;
      long int root_set = clstr_set.find(column_sp[0]);
      for (size_t nt_elem_idx = 1UL; nt_elem_idx < column_sp.size(); nt_elem_idx++)
      {
         if (column_sp[nt_elem_idx] == -1L)
            break;

         retcode = clstr_set.set_union_opt(root_set, column_sp[nt_elem_idx]);
         if (retcode == -1L)
            break;
         root_set = retcode;
      }
      if (retcode != -1L)
      {
         columns_chosen.push_back(effective_col_idx);
         if (columns_chosen.size() == rank)
            break;
      }
   }

   return as_pyarray(std::move(columns_chosen));
}