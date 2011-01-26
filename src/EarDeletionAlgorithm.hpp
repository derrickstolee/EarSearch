/*
 * EarDeletionAlgorithm.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef EARDELETIONALGORITHM_HPP_
#define EARDELETIONALGORITHM_HPP_

#include "nausparse.h"
#include "EarNode.hpp"

class EarDeletionAlgorithm
{
public:
	virtual ~EarDeletionAlgorithm();

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
	virtual int getCanonical(EarNode* parent, EarNode* child) = 0;
};

#endif /* EARDELETIONALGORITHM_HPP_ */
