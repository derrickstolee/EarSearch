/*
 * flows.hpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#ifndef FLOWS_HPP_
#define FLOWS_HPP_

#include "nausparse.h"

/**
 * achiveFlow
 *
 * Does the given graph achieve the requested flow without the specified ear?
 *
 * @param g an undirected sparsegraph
 * @param v1 an index of a vertex
 * @param v2 an index of another vertex
 * @param without_ear an array of vertices, -1 terminated.
 * @param max_flow (just use 2)
 */
bool achieveFlow(sparsegraph* g, int v1, int v2, int* without_ear, int max_flow);

#endif /* FLOWS_HPP_ */
