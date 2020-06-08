/*
 * BarrierSearch.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: derrickstolee
 */

#include "BarrierNode.hpp"
#include "BarrierAlgorithm.hpp"

/**
 * the recursive call for the methods below.
 */
int getMaxEdges(sparsegraph* g, int goalEdges, int curEdges, BarrierNode** b_list_truncated, int* conflict_list,
		int num_barriers_left, int depth, bool output)
{
	int max_edges = curEdges;
	bool able_to_extend = false;
	for ( int i = 0; i < num_barriers_left; i++ )
	{
		if ( conflict_list[i] >= depth && b_list_truncated[i]->getSize() > 1 )
		{
			/* do this one! */
			able_to_extend = true;
			BarrierNode* augment = b_list_truncated[i];

			makeBarrierComplete(g, augment->getArray(), augment->getSize());

			for ( int j = i + 1; j < num_barriers_left; j++ )
			{
				if ( conflict_list[j] >= depth && augment->conflicts(b_list_truncated[j]) )
				{
					/* this level is first conflict */
					conflict_list[j] = depth;
				}
				else if ( conflict_list[j] >= depth )
				{
					/* delay to next level */
					conflict_list[j] = depth + 1;
				}
			}

			int new_edges = augment->getSize() * (augment->getSize() - 1);
			int get_edges = getMaxEdges(g, goalEdges, curEdges + new_edges, &(b_list_truncated[i + 1]),
					&(conflict_list[i + 1]), num_barriers_left - (i + 1), depth + 1, output);

			if ( get_edges > max_edges )
			{
				max_edges = get_edges;
			}

			makeBarrierIndependent(g, augment->getArray(), augment->getSize());
		}
	}

	if ( !able_to_extend && output && curEdges >= goalEdges * 2 )
	{
		printModifiedGraphBarrier(g);
	}

	return max_edges;
}

/**
 * enumerateAllBarrierExtensions
 *
 * Take the given (promised to be 1-extendable) graph and
 * 	find all ways to fill the barriers so that the number
 * 	of edges in the resulting saturated elementary graph
 * 	are at least 'goalEdges'.
 *
 * Output is to stdout as a list of s6 representations.
 *
 * Uses the given graph for memory, adding and removing edges as necessary.
 *
 * Examines ALL barrier extensions, with some bounding techniques.
 */
bool enumerateAllBarrierExtensions(sparsegraph* graph, unsigned int goalEdges, BarrierNode** b_list, int num_barriers)
{
	/* result is false until one is found */
	bool result = false;

	if ( num_barriers == 0 )
	{
		if ( graph->nde >= goalEdges * 2 )
		{
			printModifiedGraphBarrier(graph);
			return true;
		}

		return false;
	}

	int* conflict_list = (int*) malloc(num_barriers * sizeof(int));
	for ( int i = 0; i < num_barriers; i++ )
	{
		conflict_list[i] = num_barriers;
	}

	getMaxEdges(graph, goalEdges, graph->nde, b_list, conflict_list, num_barriers, 0, true);

	free(conflict_list);
	conflict_list = 0;

	return result;
}

/**
 * searchAllBarrierExtensions
 *
 * Find the maximum number of edges in an elementary supergraph,
 * including if the number of vertices was increased to max_n
 * in the 1-extendable graphs.
 *
 * Examines ALL barrier extensions, with some bounding techniques.
 *
 * @param g the sparesgraph to check the maximum size.
 * @param max_n the maximum number of vertices allowed.
 */
int searchAllBarrierExtensions(sparsegraph* g, BarrierNode** b_list, int num_barriers)
{
	if ( num_barriers == 0 )
	{
		return g->nde / 2;
	}

	int* conflict_list = (int*) malloc(num_barriers * sizeof(int));
	for ( int i = 0; i < num_barriers; i++ )
	{
		conflict_list[i] = num_barriers;
	}

	int result = getMaxEdges(g, g->nde, g->nde, b_list, conflict_list, num_barriers, 0, false);

	free(conflict_list);
	conflict_list = 0;

	return result / 2;
}
