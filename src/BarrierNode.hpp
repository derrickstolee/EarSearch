/*
 * BarrierNode.hpp
 *
 *  Created on: Feb 22, 2011
 *      Author: derrickstolee
 */

#ifndef BARRIERNODE_HPP_
#define BARRIERNODE_HPP_

#include "nausparse.h"
#include "Set.hpp"

class BarrierNode
{
protected:
	/**
	 * size -- the number of vertices
	 * 	AND the number of components in H-B.
	 */
	int size;

	/**
	 * n -- the number of vertices in the parent graph H.
	 */
	int n;

	/**
	 * elements -- the vertices in the barrier
	 */
	Set* elements;

	/**
	 * elt_array -- an array of the elements
	 */
	int* elt_array;

	/**
	 * components -- the components of H-B, given by labels
	 * 	on the vertices of H.
	 */
	int* components;

	/**
	 * next -- the next barrier in the linked list
	 */
	BarrierNode* next;

public:
	/**
	 * Constructor with a set of vertices and the components.
	 *
	 * This is used to construct barriers for C_2k.
	 */
	BarrierNode(Set* verts, int n, int* components);

	/**
	 * Constructor from a BarrierNode, and a set of NEW vertices from an ear.
	 *
	 * @param base the BarrierNode containing a barrier in H.
	 * @param ear_list the vertices of an ear added (in order).
	 * @param num_ears the number of ears
	 * @param ear_length the number of internal vertices in the ear.
	 * @param ear_elts the vertices to add to this barrier.
	 * @param the number of vertices in ear_elts.
	 */
	BarrierNode(BarrierNode* base, int** ear_list, int num_ears, int* ear_lengths, int usable_ear, int* ear_elts,
			int num_ear_elts);

	/**
	 * Destructor
	 */
	~BarrierNode();

	/**
	 * getElements
	 *
	 * @return the set of elements of the barrier.
	 */
	Set* getElements();

	/**
	 * getComponents
	 */
	int* getComponents();

	/**
	 * splitsEar
	 *
	 * Check if this barrier splits the given ear across two components.
	 *
	 * @param ear The ear to add to H
	 * @param ear_length the number of vertices in the ear
	 * @return True iff the endpoints of ear are in different components of B
	 */
	bool splitsEar(int* ear, int ear_length);

	/**
	 * conflicts
	 *
	 * Check if this barrier conflicts with another barrier:
	 *
	 * 1) if they intersect.
	 * 2) if b is in multiple components of H-B.
	 *
	 * @param b the other barrier.
	 */
	bool conflicts(BarrierNode* b);

	/**
	 * getArray
	 *
	 * get an array of the elements
	 */
	int* getArray();

	/**
	 * getSize
	 *
	 * Get the size of the barrier.
	 */
	int getSize();

};

/**
 * constructBarriers
 *
 * Given a list of barriers of a graph,
 * 	and a list of new ears (1 or 2 new ears),
 *  compute the barriers for the new graph.
 * This is an in-line action,
 * 	so it doesn't need the graph.
 */
BarrierNode** constructBarriers(BarrierNode** b_list, int num_barriers, int** ear_list, int* ear_length, int num_ears,
		int& num_new_barriers);

#endif /* BARRIERNODE_HPP_ */
