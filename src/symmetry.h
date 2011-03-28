/*
 * symmetry.h
 *
 *  Created on: Oct 29, 2010
 *      Author: derrickstolee
 */

#ifndef SYMMETRY_H_
#define SYMMETRY_H_

#include "nausparse.h"

/**
 * getCanonicalLabelsAndPairOrbits
 *
 * use nauty to compute the pair orbits of a graph g (with possible vertex/edge deletions)
 * as well as the canonical labels of the vertices.
 *
 * @param g the graph, g->v[i] < 0 means vertex i is deleted, g->e[i] < 0 means the edge i is deleted.
 * @param labels an array for the labels of the vertices
 * @param orbitList an array for the pair orbits to be placed.
 * @return the number of pair orbits found.
 */
int getCanonicalLabelsAndPairOrbits(sparsegraph* g, int* labels,
		int** orbitList);

#endif /* SYMMETRY_H_ */
