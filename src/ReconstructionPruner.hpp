/*
 * ReconstructionPruner.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef RECONSTRUCTIONPRUNER_HPP_
#define RECONSTRUCTIONPRUNER_HPP_

#include "nausparse.h"
#include "PruningAlgorithm.hpp"

class ReconstructionPruner : public PruningAlgorithm
{
	/**
	 * n the maximum number of vertices allowed.
	 */
	int n;

	/**
	 * logFacts
	 * Stores the list of numbers 1+log2(n!) for correct values of n.
	 */
	int* logFacts;

public:
	/**
	 * Constructor
	 *
	 * @param n the maximum number of vertices allowed.
	 */
	ReconstructionPruner(int n);

	/**
	 * Destructor
	 */
	virtual ~ReconstructionPruner();

	/**
	 * checkPrune -- check if the given graph needs to be pruned.
	 *
	 * @param graph a sparsegraph to check.
	 * @return 1 iff the graph should be pruned.
	 */
	int checkPrune(EarNode* parent, EarNode* child);
};


#endif /* RECONSTRUCTIONPRUNER_HPP_ */
