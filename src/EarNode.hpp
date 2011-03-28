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
#include "BarrierNode.hpp"
#include "Set.hpp"

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
	 * max_verts -- the maximum number of vertices in
	 * all supergraphs. Default to max_n from search
	 */
	int max_verts;

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
	 * adj_matrix_data -- Data on the edges.
	 */
	int** adj_matrix_data;

	/**
	 * initAdjMatrix -- fill the adj_matrix_data arrays
	 */
	void initAdjMatrix();

	/**
	 * violatingPairs -- A set of pair indices which cannot be
	 * endpoints of an ear!
	 */
	Set* violatingPairs;

	/**
	 * any_adj_zero -- are any of the cells in adj_matrix_data zero?
	 */
	bool any_adj_zero;


	/**
	 * any_adj_two -- are any of the cells in adj_matrix_data at least two?
	 */
	bool any_adj_two;


	/**
	 * dom_vert -- there is a dominating vertex
	 */
	bool dom_vert;

	/**
	 * copy_of_H -- there is a copy_of_H
	 */
	bool copy_of_H;

	/**
	 * prune_called
	 */
	bool prune_called;

	/**
	 * child_data -- the GraphData objects for immediate children, used for pairwise comparison.
	 * num_child_data -- the number of GraphData objects currently filling the data.
	 * size_child_data -- the number of pointers currently allocated.
	 */
	int num_child_data;
	int size_child_data;
	GraphData** child_data;

	/**
	 * barriers -- The list of barriers of the graph
	 */
	BarrierNode** barriers;

	/**
	 * num_barriers -- the number of barriers in the graph.
	 */
	int num_barriers;

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
