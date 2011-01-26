/*
 * EnumerateChecker.hpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#ifndef ENUMERATECHECKER_HPP_
#define ENUMERATECHECKER_HPP_

#include "nausparse.h"
#include "SolutionChecker.hpp"

class EnumerateChecker : public SolutionChecker
{
protected:
	/**
	 * N the number of vertices we are looking for
	 */
	int N;

	/**
	 * minE the minimum number of edges.
	 */
	int minE;

	/**
	 * maxE the maximum number of edges.
	 */
	int maxE;

public:

	/**
	 * Constructor
	 */
	EnumerateChecker(int N, int minE, int maxE);

	/**
	 * Destructor
	 */
	virtual ~EnumerateChecker();


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

#endif /* ENUMERATECHECKER_HPP_ */
