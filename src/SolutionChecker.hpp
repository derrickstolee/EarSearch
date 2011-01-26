/*
 * SolutionChecker.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include "nausparse.h"
#include "EarNode.hpp"

#ifndef SOLUTIONCHECKER_HPP_
#define SOLUTIONCHECKER_HPP_

class SolutionChecker
{
protected:
	int max_depth;
	int* num_solutions_at;


public:
	/**
	 * constructor
	 *
	 * Initializes num_solutions_at with max_depth positions, all set to 0.
	 */
	SolutionChecker(int max_depth);

	/**
	 * Destructor
	 *
	 * Frees num_solutions_at
	 */
	virtual ~SolutionChecker();

	/**
	 * getNumSolutionsAt
	 *
	 * @param i the position for the number of solutions.
	 */
	int getNumSolutionsAt(int i);

	/**
	 * getMaxDepth
	 */
	int getMaxDepth();

	/**
	 * clearSolutions
	 *
	 * Clear the current list of solutions.
	 * This prevents muliple-counting when multiple jobs are grouped to a single process.
	 */
	void clearSolutions();

	/**
	 * isSolution -- take a graph and check for a solution.
	 * Return a string of data if it is a solution.
	 *
	 * @param parent the parent EarNode -- contains adjacent children already considered
	 * @param child the child EarNode -- all data for the current node
	 * @return a string of data, if it is a solution.  0 if not.
	 */
	virtual char* isSolution(EarNode* parent, EarNode* child, int depth) = 0;

	/**
	 * writeStatisticsData -- return a string of statistics from the solution attempts.
	 */
	virtual char* writeStatisticsData();

};


#endif /* SOLUTIONCHECKER_HPP_ */
