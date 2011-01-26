/*
 * ReconstructionDeleter.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef RECONSTRUCTIONDELETER_HPP_
#define RECONSTRUCTIONDELETER_HPP_

#include "EarDeletionAlgorithm.hpp"

class ReconstructionDeleter: public EarDeletionAlgorithm
{

	/**
	 * n the maximum number of vertices allowed.
	 */
	int n;

public:
	/**
	 * Constructor
	 *
	 * @param n the maximum number of vertices allowed.
	 */
	ReconstructionDeleter(int n);

	/**
	 * Destructor
	 */
	virtual ~ReconstructionDeleter();

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

#endif /* RECONSTRUCTIONDELETER_HPP_ */
