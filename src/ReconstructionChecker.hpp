/*
 * ReconstructionChecker.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef RECONSTRUCTIONCHECKER_HPP_
#define RECONSTRUCTIONCHECKER_HPP_

#include "nausparse.h"
#include "SolutionChecker.hpp"

class ReconstructionChecker: public SolutionChecker
{
	/**
	 * n the maximum number of vertices allowed.
	 */
	int n;

	/**
	 * numAtDepth -- stores the number of
	 * solution attempts for each check-depth.
	 */
	int* numAtDepth;

public:
	/**
	 * Constructor
	 *
	 * @param n the maximum number of vertices allowed.
	 */
	ReconstructionChecker(int n);

	/**
	 * Destructor
	 */
	virtual ~ReconstructionChecker();


	/**
	 * isSolution -- take a graph and check for a solution.
	 * Return a string of data if it is a solution.
	 *
	 * @param parent the parent EarNode -- contains adjacent children already considered
	 * @param child the child EarNode -- all data for the current node
	 * @return a string of data, if it is a solution.  0 if not.
	 */
	virtual char* isSolution(EarNode* parent, EarNode* child, int depth);


	/**
	 * writeStatisticsData -- return a string of statistics from the solution attempts.
	 */
	virtual char* writeStatisticsData();
};

#endif /* RECONSTRUCTIONCHECKER_HPP_ */
