/*
 * EarNode.hpp
 *
 *  Created on: Oct 17, 2010
 *      Author: derrickstolee
 */

#ifndef EARNODE_HPP_
#define EARNODE_HPP_

#include "nausparse.h"
#include "SearchManager.hpp"

class GraphData
{
public:
	/* number of vertices */
	int n;

	/* number of undirected edges */
	int e;

	/**
	 * deg_seq
	 *
	 * The sorted degree sequence.
	 */
	int* deg_seq;

	/**
	 * invariant
	 *
	 * A (currently unspecified) invariant function
	 * 	reconstructible from the edge-deck.
	 */
	double invariant;

	/**
	 * canon_strings
	 *
	 * The list of canonical strings for all cards.
	 */
	char** canon_strings;

	/**
	 * g
	 *
	 * The graph this data represents
	 */
	sparsegraph* g;


	/**
	 * constructor
	 *
	 * @param g a graph to build the data from.
	 * 	A deep copy of g is made.
	 */
	GraphData(sparsegraph* g);

	/**
	 * Destructor
	 */
	~GraphData();


	/**
	 * compare
	 *
	 * Compare the current graph data to another graph data.
	 *
	 * @return an integer signifying the progress made.
	 * 	-1 means they are the same!
	 *   0 means they failed the vertex check.
	 *   1 means they failed the degree sequence check.
	 *   2 means they failed the 'invariant' check.
	 *   3 means they failed the canonical strings of ear-deleted subgraphs check.
	 */
	int compare(GraphData* gd);


	/**
	 * computeDegSeq()
	 *
	 * Compute the sorted degree sequence from the graph.
	 */
	void computeDegSeq();

	/**
	 * computeInvariant()
	 *
	 * Compute the invariant based on the edge-cards.
	 */
	void computeInvariant();

	/**
	 * computeCanonStrings()
	 *
	 * Compute the sorted list of canonical strings of edge-deleted subgraphs.
	 */
	void computeCanonStrings();
};

class EarNode: public SearchNode
{
public:
	/**
	 * ear_length -- the length of the augmented ear.
	 */
	int ear_length;

	/**
	 * ear -- the byte-array description of the augmented ear.
	 */
	char* ear;

	/**
	 * num_ears -- the number of ears in the graph
	 */
	int num_ears;

	/**
	 * ear_list -- the list of ears in the graph (-1 terminated)
	 */
	char** ear_list;

	/**
	 * graph -- the graph at this node.
	 */
	sparsegraph* graph;


	/**
	 * reconstructible -- TRUE if detectably reconstructible
	 */
	bool reconstructible;

	/**
	 * pairOrbits -- the pair orbits for this graph.
	 */
	int numPairOrbits;

	int** orbitList;

	/**
	 * canonicalLabels -- the canonical labeling of the graph
	 */
	int* canonicalLabels;

	/**
	 * solution_data -- the data of a solution on this node.
	 */
	char* solution_data;

	/**
	 * extendable -- true if 1-extendable
	 */
	bool extendable;

	/**
	 * numMatchings -- number of perfect matchings for this node.
	 */
	int numMatchings;


	/**
	 * child_data -- the GraphData objects for immediate children, used for pairwise comparison.
	 * num_child_data -- the number of GraphData objects currently filling the data.
	 * size_child_data -- the number of pointers currently allocated.
	 */
	int num_child_data;
	int size_child_data;
	GraphData** child_data;

	/**
	 * n -- the number of vertices at this level
	 */
	int n;

	/**
	 * Constructor -- initialize!
	 */
	EarNode();


	/**
	 * Constructor -- initialize!
	 */
	EarNode(LONG_T label);


	/**
	 * Destructor
	 */
	virtual ~EarNode();

	/**
	 * addGraphData
	 *
	 * Create a new GraphData object for 'g' and add it to the list.
	 *
	 * The specific data in GraphData will only be filled when necessary,
	 * 	that is, when the earliest checks have failed.
	 */
	void addGraphData(sparsegraph* g);
};

#endif /* EARNODE_HPP_ */
