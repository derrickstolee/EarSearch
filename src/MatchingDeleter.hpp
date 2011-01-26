/*
 * MatchingDeleter.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef MATCHINGDELETER_HPP_
#define MATCHINGDELETER_HPP_

#include "EarDeletionAlgorithm.hpp"

class MatchingDeleter : public EarDeletionAlgorithm
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

public:
	/**
	 * Constructor
	 *
	 * @param p the number of perfect matchings we are looking for.
	 * @param n the maximum number of vertices allowed.
	 */
	MatchingDeleter(int pmin, int pmax, int n);

	/**
	 * Destructor
	 */
	virtual ~MatchingDeleter();


	/**
	 * getCanonical -- return the index of the canonical ear-deletion
	 * for the given parent/child.
	 *
	 * The child contains the necessary graph/ear information.
	 *
	 * @param parent the parent EarNode
	 * @param child the child EarNode
	 * @return the index of the ear to use.
	 */
	virtual int getCanonical(EarNode* parent, EarNode* child);
};

#endif /* MATCHINGDELETER_HPP_ */
