/*
 * EnumeratePruner.hpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#ifndef ENUMERATEPRUNER_HPP_
#define ENUMERATEPRUNER_HPP_

#include "nausparse.h"
#include "PruningAlgorithm.hpp"

class EnumeratePruner : public PruningAlgorithm
{
protected:
	/**
	 * N the number of vertices we are looking for
	 */
	int N;

	/**
	 * minE the minimum number of edges.
	 */
	int minE;

	/**
	 * maxE the maximum number of edges.
	 */
	int maxE;

public:

	/**
	 * Constructor
	 */
	EnumeratePruner(int N, int minE, int maxE);

	/**
	 * Destructor
	 */
	virtual ~EnumeratePruner();

	/**
	 * checkPrune -- check if the given graph needs to be pruned.
	 *
	 * @param graph a sparsegraph to check.
	 * @return 1 iff the graph should be pruned.
	 */
	int checkPrune(EarNode* parent, EarNode* child);
};

#endif /* ENUMERATEPRUNER_HPP_ */
