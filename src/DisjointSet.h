/***********************************************************************************************************************
 * @file    DisjointSet.h
 * @author  Imanol Etxezarreta (ietxezarretam@gmail.com)
 * 
 * @brief   C++ interface of the disjoint-set DSA class. Taken from 
 *          https://www.geeksforgeeks.org/introduction-to-disjoint-set-data-structure-or-union-find-algorithm/ and may
 *          be subject to some changes depending the necessities of bpbp.
 * 
 * @version 0.1
 * @date    08/05/2024
 * 
 * @copyright Copyright (c) 2024
 * 
 **********************************************************************************************************************/
#ifndef DISJOINT_SET_H_
#define DISJOINT_SET_H_

class DisjSet
{
   private:
      int *rank;
      int *parent;
      int n; 

   public: 
      // Constructor to create and 
      // initialize sets of n items 
      DisjSet(int n);

      // Creates n single item sets 
      void make_set();

      // Finds set of given item x 
      int find(int x);

      // Do union of two sets by rank represented 
      // by x and y. 
      void set_union(int x, int y);
};

#endif // DISJOINT_SET_H_