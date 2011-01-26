/*
 * EnumerateDeleter.hpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#ifndef ENUMERATEDELETER_HPP_
#define ENUMERATEDELETER_HPP_

#include "EarDeletionAlgorithm.hpp"

class EnumerateDeleter : public EarDeletionAlgorithm
{
public:
	/**
	 * Constructor
	 */
	EnumerateDeleter();

	/**
	 * Destructor
	 */
	virtual ~EnumerateDeleter();


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

#endif /* ENUMERATEDELETER_HPP_ */
