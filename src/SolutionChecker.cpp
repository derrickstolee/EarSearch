/*
 * SolutionChecker.cpp
 *
 *  Created on: Jan 20, 2011
 *      Author: derrickstolee
 */

#include "nausparse.h"
#include "SolutionChecker.hpp"
#include <stdlib.h>
#include <string.h>

/**
 * constructor
 *
 * Initializes num_solutions_at with max_depth positions, all set to 0.
 */
SolutionChecker::SolutionChecker(int max_depth)
{
	this->max_depth = max_depth;
	this->require_all_siblings = false;
	this->num_solutions_at = (int*) malloc(max_depth * sizeof(int));
	bzero(this->num_solutions_at, max_depth * sizeof(int));
}

/**
 * Destructor
 *
 * Frees num_solutions_at
 */
SolutionChecker::~SolutionChecker()
{
	if ( this->num_solutions_at != 0 )
	{
		free(this->num_solutions_at);
		this->num_solutions_at = 0;
	}
}

/**
 * getNumSolutionsAt
 *
 * @param i the position for the number of solutions.
 */
int SolutionChecker::getNumSolutionsAt(int i)
{
	if ( i < 0 || i >= this->max_depth )
	{
		return 0;
	}

	return this->num_solutions_at[i];
}

/**
 * getMaxDepth
 */
int SolutionChecker::getMaxDepth()
{
	return this->max_depth;
}

/**
 * clearSolutions
 *
 * Clear the current list of solutions.
 * This prevents muliple-counting when multiple jobs are grouped to a single process.
 */
void SolutionChecker::clearSolutions()
{
	bzero(this->num_solutions_at, this->max_depth * sizeof(int));
}

/**
 * writeSolutionData -- return a string of statistics from the solution attempts.
 */
char* SolutionChecker::writeStatisticsData()
{
	char* buffer = (char*) malloc(100);

	sprintf(buffer, "\n");

	return buffer;
}
