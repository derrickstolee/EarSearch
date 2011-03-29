/*
 * ReconstructionPruner.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <math.h>
#include "ReconstructionPruner.hpp"

/**
 * Constructor
 *
 * @param n the maximum number of vertices allowed.
 */
ReconstructionPruner::ReconstructionPruner(int n)
{
	this->n = n;

	this->logFacts = (int*) malloc((n + 1) * sizeof(int));

	this->logFacts[0] = 0;
	this->logFacts[1] = 0;
	for ( int i = 2; i <= n; i++ )
	{
		/* check that the graph has at most 1 + log2(n!) edges */
		double nfact = 1.0;

		for ( double j = 2; j <= i; j++ )
		{
			nfact *= j;
		}

		/* compute the log-base-2 */
		double lognfact = log2(nfact) + 1;

		this->logFacts[i] = (int) lognfact;
	}

}

/**
 * Destructor
 */
ReconstructionPruner::~ReconstructionPruner()
{
	free(this->logFacts);
	this->logFacts = 0;
}

/**
 * checkPrune -- check if the given graph needs to be pruned.
 *
 * @param graph a sparsegraph to check.
 * @return 1 iff the graph should be pruned.
 */
int ReconstructionPruner::checkPrune(EarNode* parent, EarNode* child)
{
	if ( child->graph->nv > this->n )
	{
		return 1;
	}

	/* if there are more edges than we want on the N-vertex graph, prune. */
	if ( child->graph->nde > 2 * this->logFacts[this->n] )
	{
		/* we are too dense! */
		return 1;
	}

	return 0;
}
