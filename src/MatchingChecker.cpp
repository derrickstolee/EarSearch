/*
 * MatchingChecker.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include "gtools.h"
#include "MatchingChecker.hpp"
#include "permcount.h"
#include "BarrierAlgorithm.hpp"

/**
 * Constructor
 *
 * @param p the number of perfect matchings we are looking for.
 * @param n the maximum number of vertices allowed.
 */
MatchingChecker::MatchingChecker(int pmin, int pmax, int n, int c) :
	SolutionChecker(2 * pmax + n)
{
	this->pmin = pmin;
	this->pmax = pmax;
	this->n = n;
	this->c = c;
}

/**
 * Destructor
 */
MatchingChecker::~MatchingChecker()
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
char* MatchingChecker::isSolution(EarNode* parent, EarNode* child, int depth)
{
	sparsegraph* g = child->graph;

	if ( g == 0 )
	{
		return 0;
	}

	int num_matchings = countPM(g, this->pmax);

	if ( num_matchings >= this->pmin && num_matchings <= this->pmax )
	{
		/* first, count POSSIBLE size of maximal barriers */
		int n = g->nv;
		int n2choose2 = (n / 2) * (n / 2 - 1);

		/* if 2|E| + 2(n/2 choose 2) <= 2(n^2/4), not enough edges possible! */
		int num_edges_have = (g->nde + n2choose2) / 2;
		int num_to_attain = (n * n) / 4 + this->c;

		/* the enumerate method outputs all graphs which attain the constant */
		bool result = enumerateOptimalBarrierExtensions(g, num_to_attain);

		if ( result == true )
		{
			printf("--p=%d\tn=%d\n", num_matchings, g->nv, (g->nde / 2));
			/* AND MAXIMUM WAY TO ADD FORBIDDEN EDGES */

			this->num_solutions_at[depth] = this->num_solutions_at[depth] + 1;
			return "\n";
		}
		else
		{
			return 0;
		}
	}

	return 0;
}
