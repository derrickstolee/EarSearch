/*
 * EnumeratePruner.cpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include "EnumeratePruner.hpp"

/**
 * Constructor
 */
EnumeratePruner::EnumeratePruner(int N, int minE, int maxE)
{
	this->N = N;
	this->minE = minE;
	this->maxE = maxE;
}

/**
 * Destructor
 */
EnumeratePruner::~EnumeratePruner()
{

}

/**
 * checkPrune -- check if the given graph needs to be pruned.
 *
 * @param graph a sparsegraph to check.
 * @return 1 iff the graph should be pruned.
 */
int EnumeratePruner::checkPrune(EarNode* parent, EarNode* child)
{
	if ( child->graph == 0 )
	{
		return 0;
	}

	if ( child->graph->nv > this->N )
	{
		/* too many vertices */
		return 1;
	}
	else if ( child->graph->nv < this->N && 2 + child->graph->nde + 2
			* (this->N - child->graph->nv) > 2 * this->maxE )
	{
		/* we need more vertices, so at least one ear (+2 edges) */
		/* and more edges (1 for every new vertex) */
		return 1;
	}
	else if ( child->graph->nde > 2 * this->maxE )
	{
		return 1;
	}

	return 0;
}
