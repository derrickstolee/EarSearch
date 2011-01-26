/*
 * MatchingChecker.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */


#ifndef MATCHINGCHECKER_HPP_
#define MATCHINGCHECKER_HPP_

#include "nausparse.h"
#include "SolutionChecker.hpp"

class MatchingChecker : public SolutionChecker
{
	/**
	 * p the number of perfect matchings we are looking for.
	 */

	int pmin;
	int pmax;

	/**
	 * n the maximum number of vertices allowed.
	 */
	int n;

	/**
	 * c the constant we are attempting to attain
	 */
	int c;

public:
	/**
	 * Constructor
	 *
	 * @param p the number of perfect matchings we are looking for.
	 * @param n the maximum number of vertices allowed.
	 * @param c the constant wea re attempting to attain.
	 */
	MatchingChecker(int pmin, int pmax, int n, int c);

	/**
	 * Destructor
	 */
	virtual ~MatchingChecker();


	/**
	 * isSolution -- take a graph and check for a solution.
	 * Return a string of data if it is a solution.
	 *
	 * @param parent the parent EarNode -- contains adjacent children already considered
	 * @param child the child EarNode -- all data for the current node
	 * @return a string of data, if it is a solution.  0 if not.
	 */
	virtual char* isSolution(EarNode* parent, EarNode* child, int depth);
};

#endif /* MATCHINGCHECKER_HPP_ */
