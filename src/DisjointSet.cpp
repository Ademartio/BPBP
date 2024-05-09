/***********************************************************************************************************************
 * @file    DisjointSet.cpp
 * @author  Imanol Etxezarreta (ietxezarretam@gmail.com)
 * 
 * @brief   C++ implementation of the disjoint-set DSA. Taken from 
 *          https://www.geeksforgeeks.org/introduction-to-disjoint-set-data-structure-or-union-find-algorithm/ and may
 *          be subject to some changes depending the necessities of bpbp.
 * 
 * @version 0.1
 * @date    08/05/2024
 * 
 * @copyright Copyright (c) 2024
 * 
 **********************************************************************************************************************/

#include "DisjointSet.h"

DisjSet::DisjSet(int const & n) 
{
   rank = new int[n];
   parent = new int[n];
   this->n = n;
   make_set();
}

// Creates n single item sets
void DisjSet::make_set()
{
   for (int i = 0; i < n; i++) {
      parent[i] = i;
   }
}

// Finds set of given item x
int DisjSet::find(int const & x)
{
   // Finds the representative of the set
   // that x is an element of
   if (parent[x] != x) {

      // if x is not the parent of itself
      // Then x is not the representative of
      // his set,
      parent[x] = find(parent[x]);

      // so we recursively call Find on its parent
      // and move i's node directly under the
      // representative of this set
   }

   return parent[x];
}

void DisjSet::set_parent(int const & x, int const & new_parent)
{
   parent[x] = new_parent;
}

int DisjSet::get_rank(int const & x)
{
   return rank[x];
}

void DisjSet::increase_rank(int const & x)
{
   rank[x] += 1;
}

// Do union of two sets by rank represented
// by x and y.
void DisjSet::set_union(int const & x, int const & y)
{
   // Find current sets of x and y
   int xset = find(x);
   int yset = find(y);

   // If they are already in same set
   if (xset == yset)
      return;

   // Put smaller ranked item under
   // bigger ranked item if ranks are
   // different
   if (rank[xset] < rank[yset]) {
      parent[xset] = yset;
   }
   else if (rank[xset] > rank[yset]) {
      parent[yset] = xset;
   }

   // If ranks are same, then increment
   // rank.
   else {
      parent[yset] = xset;
      rank[xset] = rank[xset] + 1;
   }
}