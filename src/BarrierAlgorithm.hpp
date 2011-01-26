/*
 * BarrierAlgorithm.hpp
 *
 *  Created on: Oct 20, 2010
 *      Author: derrickstolee
 */

#ifndef BARRIERALGORITHM_HPP_
#define BARRIERALGORITHM_HPP_

#include "nausparse.h"

/**
 * enumerateOptimalBarrierExtensions
 *
 * Take the given (promised to be 1-extendable) graph and
 * 	find all ways to fill the barriers so that the number
 * 	of edges in the resulting saturated elementary graph
 * 	are at least 'goalEdges'.
 *
 * Output is to stdout as a list of s6 representations.
 *
 * Uses the given graph for memory, adding and removing edges as necessary.
 */
bool enumerateOptimalBarrierExtensions(sparsegraph* graph, int goalEdges);

/**
 * getMaximumElementarySize
 *
 * Find the maximum number of edges in an elementary supergraph,
 * including if the number of vertices was increased to max_n
 * in the 1-extendable graphs.
 *
 * @param g the sparesgraph to check the maximum size.
 * @param max_n the maximum number of vertices allowed.
 */
int getMaximumElementarySize(sparsegraph* g);


/**
 * is1Extendable
 *
 * Return true iff the graph is 1-extendable.
 * Checks that every edge is in some perfect matching.
 *
 * Does not count vertices with v[i] < 0.
 */
bool is1Extendable(sparsegraph* g);


#endif /* BARRIERALGORITHM_HPP_ */
