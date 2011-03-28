/*
 * BarrierSearch.hpp
 *
 *  Created on: Feb 24, 2011
 *      Author: derrickstolee
 */

#ifndef BARRIERSEARCH_HPP_
#define BARRIERSEARCH_HPP_

#include "nausparse.h"
#include "BarrierNode.hpp"

/**
 * enumerateAllBarrierExtensions
 *
 * Take the given (promised to be 1-extendable) graph and
 * 	find all ways to fill the barriers so that the number
 * 	of edges in the resulting saturated elementary graph
 * 	are at least 'goalEdges'.
 *
 * Output is to stdout as a list of s6 representations.
 *
 * Uses the given graph for memory, adding and removing edges as necessary.
 *
 * Examines ALL barrier extensions, with some bounding techniques.
 */
bool enumerateAllBarrierExtensions(sparsegraph* graph, int goalEdges, BarrierNode** b_list, int num_barriers);


/**
 * searchAllBarrierExtensions
 *
 * Find the maximum number of edges in an elementary supergraph,
 * including if the number of vertices was increased to max_n
 * in the 1-extendable graphs.
 *
 * Examines ALL barrier extensions, with some bounding techniques.
 *
 * @param g the sparesgraph to check the maximum size.
 * @param max_n the maximum number of vertices allowed.
 */
int searchAllBarrierExtensions(sparsegraph* g, BarrierNode** b_list, int num_barriers);

#endif /* BARRIERSEARCH_HPP_ */
