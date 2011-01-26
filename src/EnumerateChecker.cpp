/*
 * EnumerateChecker.cpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include "gtools.h"
#include "EnumerateChecker.hpp"

/**
 * Constructor
 */
EnumerateChecker::EnumerateChecker(int N, int minE, int maxE) :
	SolutionChecker(maxE)
{
	this->N = N;
	this->minE = minE;
	this->maxE = maxE;
}

/**
 * Destructor
 */
EnumerateChecker::~EnumerateChecker()
{

}

/**
 * isSolution -- take a graph and check for a solution.
 * Return a string of data if it is a solution.
 *
 * @param parent the parent EarNode -- contains adjacent children already considered
 * @param child the child EarNode -- all data for the current node
 * @return a string of data, if it is a solution.  0 if not.
 */
char* EnumerateChecker::isSolution(EarNode* parent, EarNode* child, int depth)
{
	sparsegraph* graph = child->graph;

	if ( graph->nv == this->N && 2 * this->minE <= graph->nde && graph->nde
			<= 2 * this->maxE )
	{
		/* only count! */
		this->num_solutions_at[depth] = this->num_solutions_at[depth] + 1;
		return 0;
	}

	return 0;
}
