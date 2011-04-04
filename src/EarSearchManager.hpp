/*
 * EarSearchManager.hpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#ifndef EARSEARCHMANAGER_HPP_
#define EARSEARCHMANAGER_HPP_

#include "nausparse.h"
#include "SearchManager.hpp"
#include "EarNode.hpp"
#include "PruningAlgorithm.hpp"
#include "SolutionChecker.hpp"
#include "EarDeletionAlgorithm.hpp"

class EarSearchManager: public SearchManager
{
protected:
	typedef SearchManager super;

	PruningAlgorithm* pruner;
	SolutionChecker* checker;
	EarDeletionAlgorithm* deleter;

	/**
	 * max_n -- maximum size
	 */
	int max_n;

	/**
	 * require_all_siblings
	 *
	 * If TRUE, the pushTo method will generate all previous siblings
	 * before continuing.
	 *
	 * This is necessary for the reconstruction application
	 * during the execution of partial jobs.
	 */
	bool require_all_siblings;


	/**
	 * augment
	 *
	 * Given a sparsegraph, create a new one with a new ear
	 * 	of the given length between the two vertices.
	 */
	sparsegraph* augment(sparsegraph* graph, int length, int v1, int v2);

public:
	/**
	 * Constructor
	 *
	 * @param max_n the maximum number of vertices to consider.
	 * @param algorithm the algorithm for checking pruning.
	 * @param checker the algorithm for checking solutions.
	 * @param deleter the algorithm for checking canonical deletions.
	 */
	EarSearchManager(int max_n, PruningAlgorithm* algorithm, SolutionChecker* checker,
			EarDeletionAlgorithm* deleter);

	/**
	 * Destructor
	 */
	virtual ~EarSearchManager();

	/**
	 * importArguments -- take the command line arguments
	 * 	and convert them into options.
	 *
	 * This includes the following options:
	 * 	mode: generate or run
	 *  stages: -s [count] [s0] [s1] ... [sk]
	 *  killtime: -k [killtime]
	 *  maxdepth: -m [maxdepth]
	 *
	 *  It is set as virtual so that one can strip
	 *   custom arguments, if necessary.
	 */
	virtual void importArguments(int argc, char** argv);

	/**
	 * pushNext -- deepen the search to the next child
	 * 	of the current node.
	 *
	 * @return the label for the new node. -1 if none.
	 */
	virtual LONG_T pushNext();

	/**
	 * pushTo -- deepen the search to the specified child
	 * 	of the current node.
	 *
	 * @param child the specified label for the new node
	 * @return the label for the new node. -1 if none, or failed.
	 */
	virtual LONG_T pushTo(LONG_T child);

	/**
	 * pop -- remove the current node and move up the tree.
	 *
	 * @return the label of the node after the pop.
	 * 		This return value is used for validation purposes
	 * 		to check proper implementation of push*() and pop().
	 */
	virtual LONG_T pop();

	/**
	 * prune -- Perform a check to see if this node should be pruned.
	 *
	 * @return 0 if no prune should occur, 1 if prune should occur.
	 */
	virtual int prune();

	/**
	 * isSolution -- Perform a check to see if a solution exists
	 * 		at this point.
	 *
	 * @return 0 if no solution is found, 1 if a solution is found.
	 */
	virtual int isSolution();

	/**
	 * writeSolution -- create a buffer that contains a
	 * 	description of the solution.
	 *
	 * Solution strings start with S.
	 *
	 * @return a string of data allocated with malloc().
	 * 	It will be deleted using free().
	 */
	virtual char* writeSolution();

	/**
	 * writeStatistics -- create a buffer that contains a
	 * 	description of the solution.
	 *
	 * Statistics take the following format in each line:
	 *
	 * T [TYPE] [ID] [VALUE]
	 *
	 * @return a string of data allocated with malloc().
	 * 	It will be deleted using free().
	 */
	virtual char* writeStatistics();
};

#endif /* EARSEARCHMANAGER_HPP_ */
