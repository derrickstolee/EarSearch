/*
 * PruningAlgorithm.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef PRUNINGALGORITHM_HPP_
#define PRUNINGALGORITHM_HPP_

#include "nausparse.h"
#include "EarNode.hpp"

class PruningAlgorithm
{
public:
	virtual ~PruningAlgorithm();

	/**
	 * checkPrune -- check if the given graph needs to be pruned.
	 *
	 * @return 1 iff the graph should be pruned.
	 */
	virtual int checkPrune(EarNode* parent, EarNode* child) = 0;
};

#endif /* PRUNINGALGORITHM_HPP_ */
