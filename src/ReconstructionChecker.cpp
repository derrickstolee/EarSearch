/*
 * ReconstructionChecker.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gtools.h"
#include "ReconstructionChecker.hpp"

/**
 * Constructor
 *
 * @param n the maximum number of vertices allowed.
 */
ReconstructionChecker::ReconstructionChecker(int n) :
	SolutionChecker(n * n)
{
	this->n = n;

	this->numAtDepth = (int*) malloc(10 * sizeof(int));
	bzero(this->numAtDepth, 10 * sizeof(int));
}

/**
 * Destructor
 */
ReconstructionChecker::~ReconstructionChecker()
{
	free(this->numAtDepth);
	this->numAtDepth = 0;
}

/**
 * isSolution -- take a graph and check for a solution.
 * Return a string of data if it is a solution.
 *
 * @param parent the parent EarNode -- contains adjacent children already considered
 * @param child the child EarNode -- all data for the current node
 * @return a string of data, if it is a solution.  0 if not.
 */
char* ReconstructionChecker::isSolution(EarNode* parent, EarNode* child,
		int depth)
{
	/* For checking, we actually want to see if this graph is the canonical card for graphs  */
	if ( child->reconstructible || parent->num_child_data <= 0 )
	{
		return 0;
	}

	GraphData* c_data = parent->child_data[parent->num_child_data - 1];
	for ( int i = 0; i < parent->num_child_data - 1; i++ )
	{
		int compare_result = parent->child_data[i]->compare(c_data);

		/* TODO: collect statistics on the result */

		if ( compare_result < 0 )
		{
			/* A COUNTEREXAMPLE! */
			char* buffer = (char*) malloc(1000);

			sprintf(buffer, "%s%s\n", sgtos6(parent->child_data[i]->g), sgtos6(
					c_data->g));

			this->num_solutions_at[depth] = this->num_solutions_at[depth] + 1;

			return buffer;
		}
		else
		{
			/* store this attempt */
			(this->numAtDepth[compare_result])++;
		}
	}

	return 0;
}

/**
 * writeStatisticsData -- return a string of statistics from the solution attempts.
 */
char* ReconstructionChecker::writeStatisticsData()
{
	char* buffer = (char*) malloc(1000);

	sprintf(
			buffer,
			"T SUM CHECK_DEPTH_0 %d\nT SUM CHECK_DEPTH_1 %d\nT SUM CHECK_DEPTH_2 %d\nT SUM CHECK_DEPTH_3 %d\n",
			this->numAtDepth[0], this->numAtDepth[1], this->numAtDepth[2],
			this->numAtDepth[3]);

	return buffer;
}
