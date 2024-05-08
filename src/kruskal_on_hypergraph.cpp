/***********************************************************************************************************************
 * @file    bpbp.cpp
 * @author  Imanol Etxezarreta (ietxezarretam@gmail.com)
 * 
 * @brief   Simple cpp implementation of kruskal_on_hypergraph() function to try to improve performance.
 *          Most probably will use BGL (Boost Graph Library)
 * 
 * @version 0.1
 * @date    2024-04-25
 * 
 * @copyright Copyright (c) 2024
 * 
 **********************************************************************************************************************/

#include <iostream>
#include <algorithm>

#include "kruskal_on_hypergraph.h"

// For debug purposes
void printMatrix(const std::vector<std::vector<bool>>& matrix) {
   for (const auto& row : matrix) {
      for (bool element : row) {
         std::cout << element << " ";
      }
      std::cout << std::endl;
   }
}


static std::vector<std::vector<bool>> conditionMatrix(std::vector<std::vector<bool>>const & hog)
{
   const size_t hog_rows = hog.size();
   const size_t hog_cols = (hog_rows > 0) ? hog[0].size() : 0;

   std::vector<std::vector<bool>> H(hog_cols, std::vector<bool>(hog_rows + 2));

   for (size_t i = 0; i < hog_cols; i++)
   {
      std::vector<size_t> ones_idx;
      for (size_t j = 0; j < hog_rows; j++)
      {
         H[i][j] = hog[j][i];
         if (hog[j][i] == 1)
         {
            ones_idx.push_back(j);
         }
      }

      if (ones_idx.size() == 1)
      {
         // We do not care about integer division, in fact, we take advantage
         // of it, because we want to floor the result. We save the extra cost
         // of calling the std::floor routine because hog_rows and the index value
         // will always be positive, so the truncation towards 0 is the same as
         // flooring.
         if (ones_idx[0] < hog_rows / 2)
         {
            H[i][hog_rows] = 1;
         }
         else
         {
            H[i][hog_rows+1] = 1;
         }
         ones_idx.clear();
      }
   }

   return H;
}


static void setClusters(std::vector<bool> const & row, 
               std::vector<size_t>& clstr_vec,
               size_t const & clstr_num)
{
   auto row_it = std::find_if(row.begin(), row.end(), 
                                 [](bool idx)
                                 {
                                    return idx == 1;
                                 }
                              );

   while (row_it != row.end())
   {
      size_t idx = std::distance(row.begin(), row_it);
      clstr_vec[idx] = clstr_num;

      row_it++;
      row_it = std::find_if(row_it, row.end(), 
                              [](bool idx)
                              {
                                 return idx == 1;
                              }
                           );
   }
}


static std::vector<size_t> getRowNonZeroValues(std::vector<bool> const & row,
                                                std::vector<size_t> const & clstr_vec)
{
   std::vector<size_t> non_zero_clstr_vals;

   auto row_it = std::find_if(row.begin(), row.end(), 
                                 [](bool idx)
                                 {
                                    return idx == 1;
                                 }
                              );

   while (row_it != row.end())
   {
      size_t idx = std::distance(row.begin(), row_it);
      if (clstr_vec[idx] != 0)
      {
         non_zero_clstr_vals.push_back(clstr_vec[idx]);
      }

      row_it++;
      row_it = std::find_if(row_it, row.end(), 
                              [](bool idx)
                              {
                                 return idx == 1;
                              }
                           );
   }

   return non_zero_clstr_vals;
}


static bool checkLoop(std::vector<size_t> const & non_zero_clstr_vals)
{
   bool ret_val = false;
   std::vector<size_t> tmp_buf(non_zero_clstr_vals);

   std::sort(tmp_buf.begin(), tmp_buf.end());

   for (auto it_tmp_buf = tmp_buf.begin(); it_tmp_buf != tmp_buf.end(); )
   {
      int num_times = std::count(it_tmp_buf, tmp_buf.end(), *it_tmp_buf);
      if (num_times > 1)
      {
         ret_val = true;
         break;
      }
      // jump
      it_tmp_buf += num_times;
   }

   return ret_val;
}


static void unifyClusters(std::vector<bool> const & row,
                           std::vector<size_t>& clstr_vec,
                           size_t const & clstr_num,
                           std::vector<size_t> const & non_zero_clstr_vals)
{
   setClusters(row, clstr_vec, clstr_num);

   for (size_t i = 0; i < non_zero_clstr_vals.size(); ++i)
   {
      std::vector<size_t>::iterator it_clstr = std::find(clstr_vec.begin(), clstr_vec.end(), non_zero_clstr_vals[i]);
      while (it_clstr != clstr_vec.end())
      {
         *it_clstr = clstr_num;

         it_clstr++;
         it_clstr = std::find(it_clstr, clstr_vec.end(), non_zero_clstr_vals[i]);
      }
   }
}


static std::vector<std::vector<bool>> formatResult(std::vector<std::vector<bool>> const & H_square_T)
{
   // Transpose and remove last two rows...
   const size_t rows_T = H_square_T.size();
   const size_t cols_T = (rows_T > 0) ? H_square_T[0].size()-2 : 0;

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
   kruskal_on_hypergraph(std::vector<std::vector<bool>> const & hog)
{
   // Return value declaration
   std::vector<size_t> column_to_square(hog.size(), 0);
   std::vector<std::vector<bool>> H_square_T;

   const size_t hog_rows = hog.size();
   const size_t hog_cols = (hog_rows > 0) ? hog[0].size() : 0;

   std::vector<std::vector<bool>> H_T = conditionMatrix(hog);

   std::vector<size_t> cluster_array(H_T[0].size(), 0);
   size_t cluster_number = 1U;
   size_t column_number = 1U;

   H_square_T.push_back(H_T[0]);
   setClusters(H_T[0], cluster_array, cluster_number);
   cluster_number++;

   for (size_t i = 1; i < hog_cols; i++)
   {
      std::vector<bool> const & row = H_T[i];

      std::vector<size_t> non_zero_clstr_vals = getRowNonZeroValues(row, cluster_array);

      if (non_zero_clstr_vals.empty())
      {
         setClusters(row, cluster_array, cluster_number);
         cluster_number++;
      }
      else if (checkLoop(non_zero_clstr_vals))
      {
         continue;
      }
      else if (non_zero_clstr_vals.size() == 1)
      {
         setClusters(row, cluster_array, non_zero_clstr_vals[0]);
      } 
      else
      {
         unifyClusters(row, cluster_array, cluster_number, non_zero_clstr_vals);
         cluster_number++;
      }

      column_to_square[column_number] = i;
      H_square_T.push_back(row);
      column_number += 1;
      if (column_number == hog_cols)
      {
         break;
      }
   }

   //assert(column_to_square[hog.size()] != 0)
   H_square_T.push_back(std::vector<bool>(H_T[0].size(), false));

   std::vector<std::vector<bool>> H_square = formatResult(H_square_T);

   return std::make_tuple(H_square, column_to_square);
}

