/***********************************************************************************************************************
 * @file    kruskal_uf_hypergraph.cpp
 * @author  Imanol Etxezarreta (ietxezarretam@gmail.com)
 * 
 * @brief   Implementation of the algorithm developed by Ton to apply kruskal algorithm on hypergraphs with Union-Find
 *          (Disjoint-set) data structures.
 * 
 * @version 0.1
 * @date    09/05/2024
 * 
 * @copyright Copyright (c) 2024
 * 
 **********************************************************************************************************************/

#include <algorithm>
#include <iostream>
#include <numeric>

#include "uf_kruskal_on_hypergraph.h"
#include "DisjointSet.h"

template <typename T>
static std::vector<size_t> sort_indexes(std::vector<T> const &v) {

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
                        return v[i1] < v[i2];
                        }
                  );

   return idx;
}

static std::vector<std::vector<bool>> conditionMatrix(std::vector<std::vector<bool>>const & hog)
{
   const size_t hog_rows = hog.size();
   const size_t hog_cols = (hog_rows > 0) ? hog[0].size() : 0;

   std::vector<std::vector<bool>> H(hog_cols, std::vector<bool>(hog_rows, 0));

   for (size_t i = 0; i < hog_cols; i++)
   {
      std::vector<size_t> ones_idx;
      for (size_t j = 0; j < hog_rows; j++)
      {
         H[i][j] = hog[j][i];
         // if (hog[j][i] == 1)
         // {
         //    ones_idx.push_back(j);
         // }
      }

      // if (ones_idx.size() == 1)
      // {
      //    // We do not care about integer division, in fact, we take advantage
      //    // of it, because we want to floor the result. We save the extra cost
      //    // of calling the std::floor routine because hog_rows and the index value
      //    // will always be positive, so the truncation towards 0 is the same as
      //    // flooring.
      //    if (ones_idx[0] < hog_rows / 2)
      //    {
      //       H[i][hog_rows] = 1;
      //    }
      //    else
      //    {
      //       H[i][hog_rows+1] = 1;
      //    }
      //    ones_idx.clear();
      // }
   }

   return H;
}

static std::vector<size_t> getRowNonZeroValues(std::vector<bool> const & row)
{
   std::vector<size_t> nt_elem_col;

   auto row_it = std::find_if(row.begin(), row.end(), 
                                 [](bool idx)
                                 {
                                    return idx == 1;
                                 }
                              );

   while (row_it != row.end())
   {
      size_t row_idx = std::distance(row.begin(), row_it);
      nt_elem_col.push_back(row_idx);


      row_it++;
      row_it = std::find_if(row_it, row.end(), 
                              [](bool idx)
                              {
                                 return idx == 1;
                              }
                           );
   }

   return nt_elem_col;
}

static std::vector<std::vector<bool>> formatResult(std::vector<std::vector<bool>> & H_square_T)
{
   // Transpose and remove last two rows...
   size_t rows_T = H_square_T.size();
   size_t cols_T = (rows_T > 0) ? H_square_T[0].size()-2 : 0;

   if (cols_T >= rows_T)
   {
      size_t rows_to_add = cols_T - rows_T + 1;
      std::vector<std::vector<bool>> tmp_vectors(rows_to_add, std::vector<bool>(cols_T, 0));
      H_square_T.insert(H_square_T.end(), tmp_vectors.begin(), tmp_vectors.end());
      // std::vector<bool> tmp_row(cols_T, 0);
      // for (size_t i = 0; i < rows_to_add; i++)
      // {
      //    H_square_T.push_back(tmp_row);
      // }
      rows_T += rows_to_add;
   }

   std::vector<std::vector<bool>> H_square(cols_T, std::vector<bool>(rows_T, false));

   for (size_t i = 0; i < rows_T; ++i)
   {
      for (size_t j = 0; j < cols_T; ++j)
      {
         H_square[j][i] = H_square_T[i][j];
      }
   }

   return H_square;
}

std::tuple<std::vector<std::vector<bool>>, std::vector<size_t>>
   uf_kruskal_on_hypergraph(std::vector<std::vector<bool>> const & hog)
{
   const size_t hog_rows = hog.size();
   const size_t hog_cols = (hog_rows > 0) ? hog[0].size() : 0;

   // Return value declaration
   std::vector<size_t> columns_chosen;
   std::vector<std::vector<bool>> H_square_T;

   //std::vector<std::vector<bool>> H_T = conditionMatrix(hog);
   std::vector<std::vector<bool>> const & H_T = hog;

   DisjSet clstr_set = DisjSet(hog_rows+2);

   for (size_t i = 0; i < hog_cols; i++)
   {
      std::vector<bool> const & row = H_T[i];
      std::vector<bool> checker(hog_rows+2, 0);
      std::vector<int> depths = {0, -1, 0};
      bool boolean_condition = true;

      std::vector<size_t> nt_elem_col = getRowNonZeroValues(row);

      for (size_t nt_elem_idx = 0; nt_elem_idx < nt_elem_col.size(); nt_elem_idx++)
      {
         int elem_root = clstr_set.find(nt_elem_col[nt_elem_idx]);
         int elem_depth = clstr_set.get_rank(elem_root);

         if (checker[elem_root] == 1)
         {
            boolean_condition = false;
            break;
         }
         checker[elem_root] = 1;

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
         std::vector<size_t> nt_checker = getRowNonZeroValues(checker);
         for (const auto & elem : nt_checker)
         {
            clstr_set.set_parent(elem, depths[0]);
         }
         columns_chosen.push_back(i);
         H_square_T.push_back(row);
         if (depths[2] == 1)
         {
            clstr_set.increase_rank(depths[0]);
         }
      }
   }

   //assert(column_to_square[hog.size()] != 0)
   H_square_T.push_back(std::vector<bool>(H_T[0].size(), false));

   std::vector<std::vector<bool>> H_square = formatResult(H_square_T);

   return std::make_tuple(H_square, columns_chosen);
}

std::vector<size_t> sort_llrs(std::vector<double> const & llrs)
{
   return sort_indexes(llrs);
}