
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

inline constexpr unsigned char operator "" _uc(unsigned long long arg) noexcept
{
   return static_cast<unsigned char>( arg );
}

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

py::array_t<size_t> koh_v2_uf(py::array_t<ssize_t, py::array::f_style> const & hog, 
                              py::array_t<float> const & llrs, 
                              size_t const & orig_hog_rows,
                              py::array_t<size_t, py::array::c_style> idxs)
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
      
      std::vector<unsigned char> checker(orig_hog_rows, 0);
      std::vector<int> depths = {0, -1, 0};
      bool boolean_condition = true;

      for (size_t nt_elem_idx = 0UL; nt_elem_idx < column_sp.size(); nt_elem_idx++)
      {
         if (column_sp[nt_elem_idx] == -1L)
            break;
         int elem_root = clstr_set.find(column_sp[nt_elem_idx]);
         int elem_depth = clstr_set.get_rank(elem_root);

         if (checker[elem_root] == 1_uc)
         {
            boolean_condition = false;
            break;
         }
         checker[elem_root] = 1_uc;

         if (elem_depth > depths[1])
         {
            depths = {elem_root, elem_depth, 0};
         }
         if (elem_depth == depths[1])
         {
            depths[2] = 1;
         }
      }

      if (boolean_condition == true)
      {
         for(size_t elem = 0UL; elem < checker.size(); elem++)
         {
            if (checker[elem] == 1_uc)
               clstr_set.set_parent(elem, depths[0]);
         }
         columns_chosen.push_back(effective_col_idx);
         if (depths[2] == 1)
         {
            clstr_set.increase_rank(depths[0]);
         }
      }
   }

   return as_pyarray(std::move(columns_chosen));
}