/*
 * MatchingPruner.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef MATCHINGPRUNER_HPP_
#define MATCHINGPRUNER_HPP_

#include "nausparse.h"
#include "EarNode.hpp"
#include "PruningAlgorithm.hpp"

class MatchingPruner : public PruningAlgorithm
{
	/**
	 * p the number of perfect matchings we are looking for.
	 */

	int pmin;
	int pmax;

	/**
	 * n the maximum number of vertices allowed.
	 */
	int n;

	int c;

public:
	/**
	 * Constructor
	 *
	 * @param p the number of perfect matchings we are looking for.
	 * @param n the maximum number of vertices allowed.
	 */
	MatchingPruner(int pmin, int pmax, int n, int c);

	/**
	 * Destructor
	 */
	virtual ~MatchingPruner();

	/**
	 * checkPrune -- check if the given graph needs to be pruned.
	 *
	 * @param graph a sparsegraph to check.
	 * @return 1 iff the graph should be pruned.
	 */
	int checkPrune(EarNode* parent, EarNode* child);
};

#endif /* MATCHINGPRUNER_HPP_ */
