/*
 * SaturationAlgorithm.hpp
 *
 *  Created on: Feb 4, 2011
 *      Author: derrickstolee
 */

#ifndef SATURATIONALGORITHM_HPP_
#define SATURATIONALGORITHM_HPP_

#include "PruningAlgorithm.hpp"
#include "SolutionChecker.hpp"

#define GRAPH_K 0x100
#define GRAPH_K_4 0x104
#define GRAPH_K_5 0x105
#define GRAPH_K_6 0x106

#define GRAPH_C 0x200
#define GRAPH_C_8 0x208
#define GRAPH_C_9 0x209
#define GRAPH_C_A 0x20A
#define GRAPH_C_B 0x20B

#define SAT_OUTPUT_SOLUTIONS 1
#define SAT_OUTPUT_ALL 2

class SaturationAlgorithm : public PruningAlgorithm, public SolutionChecker
{
protected:

	/**
	 * mode -- the mode of the saturation checker.
	 *
	 * Values given by the constants above.
	 *
	 */
	int mode;

	/**
	 * output_mode -- the output mode for the checker.
	 *
	 * Do we output only on solutions, or do we output for possible subgraphs?
	 */
	int output_mode;

	/**
	 * unique -- set to true if searching for unique saturation.
	 */
	bool unique;

public:
	/**
	 * Constructor
	 */
	SaturationAlgorithm(int mode, int output_mode, bool unique, int max_depth);

	/**
	 * Destructor
	 */
	virtual ~SaturationAlgorithm();


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
	 * checkPrune -- check if the given graph needs to be pruned.
	 *
	 * @return 1 iff the graph should be pruned.
	 */
	virtual int checkPrune(EarNode* parent, EarNode* child);

};

#endif /* SATURATIONALGORITHM_HPP_ */
