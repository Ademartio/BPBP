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
      unsigned long *parent;
      unsigned long n; 

   public: 
      // Constructor to create and 
      // initialize sets of n items 
      DisjSet(unsigned long const & n);

      // Creates n single item sets 
      void make_set();

      // Finds set of given item x 
      unsigned long find(unsigned long const & x);

      // Sets the parent of element x to new_parent.
      void set_parent(unsigned long const & x, unsigned long const & new_parent);

      // Returns the rank associated with the x value.
      int get_rank(unsigned long const & x);

      // Increases by 1 the rank of the element x.
      void increase_rank(unsigned long const & x);

      // Do union of two sets by rank represented 
      // by x and y. 
      void set_union(unsigned long const & x, unsigned long const & y);
};

#endif // DISJOINT_SET_H_