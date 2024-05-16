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

DisjSet::DisjSet(unsigned long const & n) 
{
   rank = new int[n];
   parent = new unsigned long[n];
   this->n = n;
   make_set();
}

// Creates n single item sets
void DisjSet::make_set()
{
   for (unsigned long i = 0; i < n; i++) {
      parent[i] = i;
   }
}

// Finds set of given item x
unsigned long DisjSet::find(unsigned long const & x)
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

void DisjSet::set_parent(unsigned long const & x, unsigned long const & new_parent)
{
   parent[x] = new_parent;
}

int DisjSet::get_rank(unsigned long const & x)
{
   return rank[x];
}

void DisjSet::increase_rank(unsigned long const & x)
{
   rank[x] += 1;
}

// Do union of two sets by rank represented
// by x and y.
int DisjSet::set_union(unsigned long const & x, unsigned long const & y)
{
   // Find current sets of x and y
   unsigned long xset = find(x);
   unsigned long yset = find(y);

   // If they are already in same set
   if (xset == yset)
      return -1;

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

   return 0;
}