
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>

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

template <typename T>
static std::vector<size_t> sort_indexes(py::array_t<T> const &v) {

  // initialize original index locations
   std::vector<size_t> idx(v.size());
   std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values 
   std::sort(idx.begin(), idx.end(), 
                     [&v](size_t i1, size_t i2) 
                     {
                        return v.data()[i1] < v.data()[i2];
                        }
                  );

   return idx;
}

static std::vector<size_t> getRowNonZeroValues(std::vector<unsigned char> const & col)
{
   std::vector<size_t> nt_elem_col;
   nt_elem_col.reserve(col.size());

   for (size_t i = 0UL; i < col.size(); i++)
   {
      if (col[i] == 1_uc)
      {
         nt_elem_col.push_back(i);
      }
   }

   nt_elem_col.shrink_to_fit();

   return nt_elem_col;
}

py::array_t<size_t> koh_v2(py::array_t<unsigned char, py::array::f_style> const & hog, 
                           py::array_t<float> const & llrs)
{
   const size_t hog_rows = hog.shape(0);
   const size_t hog_cols = hog.shape(1);
   
   std::vector<size_t> columns_chosen;
   columns_chosen.reserve(hog_cols);

   std::vector<size_t> sorted_idxs = sort_indexes(llrs);

   DisjSet clstr_set = DisjSet(hog_rows+2);

   for (size_t col_idx = 0UL; col_idx < sorted_idxs.size(); col_idx++)
   {
      size_t effective_col_idx = sorted_idxs.data()[col_idx];
      size_t col_offset = effective_col_idx * hog_rows;
      std::vector<unsigned char> column(hog.data() + col_offset, hog.data() + col_offset + hog_rows);
      
      std::vector<unsigned char> checker(hog_rows+2, 0);
      std::vector<int> depths = {0, -1, 0};
      bool boolean_condition = true;

      //std::vector<size_t> nt_elem_col = getRowNonZeroValues(column);

      for (size_t nt_elem_idx = 0UL; nt_elem_idx < column.size()/* nt_elem_col.size() */; nt_elem_idx++)
      {
         if (column[nt_elem_idx] == 0_uc)
            continue;
         int elem_root = clstr_set.find(/* nt_elem_col[*/nt_elem_idx/* ] */);
         int elem_depth = clstr_set.get_rank(elem_root);

         if (checker[elem_root] == 1_uc)
         {
            boolean_condition = false;
            break;
         }
         checker[elem_root] = 1_uc;

         if (elem_depth > depths[1])
         {
            depths = {elem_root, elem_depth, 0_uc};
         }
         if (elem_depth == depths[1])
         {
            depths[2] = 1_uc;
         }
      }

      if (boolean_condition == true)
      {
         // std::vector<size_t> nt_checker = getRowNonZeroValues(checker);
         // for (const auto & elem : nt_checker)
         for(size_t elem = 0UL; elem < checker.size(); elem++)
         {
            if (checker[elem] == 1_uc)
               clstr_set.set_parent(elem, depths[0]);
         }
         columns_chosen.push_back(effective_col_idx);
         if (depths[2] == 1_uc)
         {
            clstr_set.increase_rank(depths[0]);
         }
      }


   }

   return as_pyarray(std::move(columns_chosen));
}