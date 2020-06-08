/*
 * BarrierAlgorithm.cpp
 *
 *  Created on: Oct 20, 2010
 *      Author: derrickstolee
 */

#include "permcount.h"
#include "nausparse.h"
#include "gtools.h"
#include "pairs.h"
#include "BarrierAlgorithm.hpp"

void printModifiedGraphBarrier(sparsegraph* g)
{
	/* need to make a better graph! */
	sparsegraph sg;
	SG_INIT(sg);

	sg.w = 0;
	sg.wlen = 0;

	sg.v = (size_t*) malloc(g->nv * sizeof(size_t));
	sg.d = (int*) malloc(g->nv * sizeof(int));
	sg.e = (int*) malloc(g->nde * sizeof(int));

	int* rev_verts = (int*) malloc(g->nv * sizeof(int));
	int* down_verts = (int*) malloc(g->nv * sizeof(int));

	int vindex = 0;
	int eindex = 0;
	int indirect_edge_deletes = 0;

	for ( int i = 0; i < g->nv; i++ )
	{
		/* if vertex is not deleted */
		if ( g->v[i] >= 0 )
		{
			//			printf("[%d] : %d \t", vindex, i);
			rev_verts[vindex] = i;
			down_verts[i] = vindex;
			vindex++;
		}
	}

	vindex = 0;

	for ( int i = 0; i < g->nv; i++ )
	{
		/* if vertex is not deleted */
		if ( g->v[i] >= 0 )
		{
			sg.v[vindex] = eindex;
			sg.d[vindex] = 0;

			for ( int j = 0; j < g->d[i]; j++ )
			{
				/* if edge is not deleted AND other vertex is not deleted */
				int edge_val = g->e[g->v[i] + j];

				if ( edge_val >= 0 && g->v[edge_val] >= 0 )
				{
					sg.e[eindex] = down_verts[edge_val];
					sg.d[vindex] = sg.d[vindex] + 1;
					eindex++;
				}
				else
				{
					/* the vertex on the other end was deleted */
					indirect_edge_deletes++;
				}
			}

			vindex++;
		}
	}

	sg.vlen = vindex;
	sg.dlen = vindex;
	sg.elen = eindex;
	sg.nv = vindex;
	sg.nde = eindex;

	free(down_verts);
	free(rev_verts);

//	printf("--c=%d\n", (sg.nde / 2) - (sg.nv * sg.nv) / 4);
	printf("%s", sgtos6(&sg));

	SG_FREE(sg);
}

/**
 * makeBarrierComplete
 *
 */
void makeBarrierComplete(sparsegraph* graph, int* barrier, int num_elts)
{
	/* we are promised that these positions are the last few edges in the graph */
	for ( int i = 0; i < num_elts; i++ )
	{
		int vindex = 0;
		int vbase = graph->v[barrier[i]] + graph->d[barrier[i]];
		for ( int j = 0; j < num_elts; j++ )
		{
			if ( j != i )
			{
				graph->e[vbase + vindex] = barrier[j];

				for ( int k = 0; k < graph->d[barrier[i]]; k++ )
				{
					if ( graph->e[graph->v[barrier[i]] + k] == barrier[j] )
					{
						printf("--[makeBarrierComplete] Trying to add edge %d->%d but it's already there! \n",
								barrier[i], barrier[j]);
					}
				}

				vindex++;
			}
		}

		/* just add these edges! */
		graph->d[barrier[i]] = graph->d[barrier[i]] + vindex;
	}

	graph->nde = graph->nde + (num_elts * (num_elts - 1));
}

void makeBarrierIndependent(sparsegraph* graph, int* barrier, int num_elts)
{
	/* we are promised that these positions are the last few edges in the graph */
	for ( int i = 0; i < num_elts; i++ )
	{
		/* just delete these edges! */
		graph->d[barrier[i]] = graph->d[barrier[i]] - (num_elts - 1);

		int vindex = 0;
		int vbase = graph->v[barrier[i]] + graph->d[barrier[i]];
		for ( int j = 0; j < num_elts; j++ )
		{
			if ( j != i )
			{
				graph->e[vbase + vindex] = -1;
				vindex++;
			}
		}
	}

	graph->nde = graph->nde - (num_elts * (num_elts - 1));
}

void refineBarriers(sparsegraph* graph, int* num_barriers, int*** barriers, int** b_sizes)
{
	/* will allocate the arrays... */
	int* barrier_rep = (int*) malloc(graph->nv * sizeof(int));

	(*num_barriers) = 0;

	for ( int i = 0; i < graph->nv; i++ )
	{
		barrier_rep[i] = i;
		int vi = graph->v[i];
		graph->v[i] = -1;

		/* test for a lower barrier rep */
		for ( int j = 0; j < i; j++ )
		{
			if ( barrier_rep[j] != j )
			{
				/* this isn't even its own rep */
				continue;
			}

			int vj = graph->v[j];

			/* modify the graph to delete this vertex j */
			graph->v[j] = -1;

			/* IF G - i - j HAS a PM, then */
			/* there is a maximum matching of G-i which misses j */
			/* and j is in D(G-i)... */
			bool has_pm = hasPM(graph);

			/* replace this vertex j */
			graph->v[j] = vj;

			if ( !has_pm )
			{
				/* j is in EVERY maximum matching of G-i */
				/* j is in the A-set of G-i (since C = \emptyset) */
				barrier_rep[i] = j;

				break;
			}
		}

		if ( barrier_rep[i] == i )
		{
			(*num_barriers) = *num_barriers + 1;
		}

		graph->v[i] = vi;
	}

	(*barriers) = (int**) malloc(*num_barriers * sizeof(int*));
	(*b_sizes) = (int*) malloc(*num_barriers * sizeof(int));

	for ( int i = 0; i < *num_barriers; i++ )
	{
		(*barriers)[i] = (int*) malloc(graph->nv * sizeof(int));
		(*b_sizes)[i] = 0;
	}

	int cur_barrier = 0;

	for ( int i = 0; i < graph->nv; i++ )
	{
		if ( barrier_rep[i] == i )
		{
			/* fill this barrier! */
			for ( int j = i; j < graph->nv; j++ )
			{
				if ( barrier_rep[j] == i )
				{
					(*barriers)[cur_barrier][(*b_sizes)[cur_barrier]] = j;
					((*b_sizes)[cur_barrier]) = (*b_sizes)[cur_barrier] + 1;
				}
			}

			cur_barrier++;
		}
	}

	/* now, we are done! */
	free(barrier_rep);
}

/**
 * fillBarriers
 *
 * A recursive algorithm for checking all possible ways to fill the barriers given.
 *
 * The barriers listed have NOT been filled in.
 *
 * We pick one barrier to fill, fill it, refine the new barriers, and recurse.
 *
 * @return true if it finds at least one.
 */
bool fillBarriers(sparsegraph* graph, unsigned int goalEdges, int num_barriers, int** barriers, int* b_sizes)
{

	bool all_complete = true;
	int max_new_edges = 0;
	int max_empty_barrier = 0;

	for ( int i = 0; i < num_barriers; i++ )
	{
		if ( b_sizes[i] > 1 )
		{
			/* only need to test the first and last elements in the barrier */
			int vj = barriers[i][0];
			int vk = barriers[i][1];

			/* if the barrier is complete, the LAST edge is pointing to the last
			 * 	barrier element
			 */
			bool are_adjacent = false;

			for ( int k = 0; k < graph->d[vj]; k++ )
			{
				if ( graph->e[graph->v[vj] + k] == vk )
				{
					/* this barrier is already complete! */

					are_adjacent = true;
					break;
				}
			}

			if ( !are_adjacent )
			{
				all_complete = false;

				max_new_edges += b_sizes[i] * (b_sizes[i] - 1);

				if ( b_sizes[i] > max_empty_barrier )
				{
					max_empty_barrier = b_sizes[i];
				}
			}
		}
	}

	if ( all_complete )
	{
		if ( graph->nde >= 2 * goalEdges )
		{
			printModifiedGraphBarrier(graph);
			return true;
		}

		//		printf("\t\t\t all complete, but not enough! %s", sgtos6(graph));
		/* not enough edges! */
		return false;
	}

	if ( max_new_edges + graph->nde < 2 * goalEdges )
	{
		/* not enough edges possible! */
		//		printf("\t\t\t partially full, but not enough! %s", sgtos6(graph));
		return false;
	}

	bool result = false;

	/* otherwise, we need to fill one in! */
	for ( int i = 0; i < num_barriers; i++ )
	{
		if ( b_sizes[i] == max_empty_barrier )
		{
			/* only need to test the first and last elements in the barrier */
			int vj = barriers[i][0];
			int vk = barriers[i][1];

			bool are_adjacent = false;

			for ( int k = 0; k < graph->d[vj]; k++ )
			{
				if ( graph->e[graph->v[vj] + k] == vk )
				{
					/* this barrier is already complete! */

					are_adjacent = true;
					break;
				}
			}

			if ( are_adjacent )
			{
				/* skip this barrier */
				continue;
			}

			/* make barrier complete */
			makeBarrierComplete(graph, barriers[i], b_sizes[i]);

			/* try filling this one in */
			int new_num_barriers;
			int** new_barriers;
			int* new_b_sizes;

			refineBarriers(graph, &new_num_barriers, &new_barriers, &new_b_sizes);

			/* recurse */
			result = fillBarriers(graph, goalEdges, new_num_barriers, new_barriers, new_b_sizes) || result;

			/* free, and reset graph */
			for ( int j = 0; j < new_num_barriers; j++ )
			{
				free(new_barriers[j]);
			}
			free(new_b_sizes);
			free(new_barriers);

			/* make barrier independent */
			makeBarrierIndependent(graph, barriers[i], b_sizes[i]);
		}
	}

	return result;
}

/**
 * enumerateOptimalBarrierExtensions
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
 * @return true if it finds at least one.
 */
bool enumerateOptimalBarrierExtensions(sparsegraph* graph, int goalEdges)
{
	if ( !is1Extendable(graph) )
	{
		//		printf("----[enumerateOptimalBarrierExtensions] \t\t Not 1-extendable!\n");
		//		printModifiedGraphBarrier(graph);

		/* not 1-extendable, is NOT a solution! */
		return false;
	}

	/* find the list of current barriers */
	int new_num_barriers;
	int** new_barriers;
	int* new_b_sizes;

	refineBarriers(graph, &new_num_barriers, &new_barriers, &new_b_sizes);

	/* recurse */
	bool result = fillBarriers(graph, goalEdges, new_num_barriers, new_barriers, new_b_sizes);

	/* free, and reset graph */
	for ( int j = 0; j < new_num_barriers; j++ )
	{
		free(new_barriers[j]);
	}

	free(new_barriers);
	free(new_b_sizes);

	return result;
}

/**
 * is1Extendable
 *
 * Return true iff the graph is 1-extendable.
 * Checks that every edge is in some perfect matching.
 *
 * Does not count vertices with v[i] < 0.
 */
bool is1Extendable(sparsegraph* g)
{
	int* labels = (int*) malloc(g->nv * sizeof(int));
	bool* edge_covered = (bool*) malloc(g->nv * g->nv * sizeof(bool));

	for ( int i = 0; i < g->nv; i++ )
	{
		for ( int j = 0; j < g->nv; j++ )
		{
			if ( i != j )
			{
				edge_covered[pairToIndex(g->nv, i, j)] = false;
			}
		}
	}

	bool result = true;

	for ( int i = 0; i < g->nv; i++ )
	{
		if ( g->v[i] < 0 )
		{
			/* vertex is deleted */
			continue;
		}

		int vi = g->v[i];

		/* delete i */
		g->v[i] = -1;

		for ( int j = 0; j < g->d[i]; j++ )
		{
			int vert2 = g->e[vi + j];

			if ( vert2 < 0 )
			{
				/* edge is deleted */
				continue;
			}

			if ( g->v[vert2] < 0 )
			{
				/* vertex is deleted */
				continue;
			}

			int edge = pairToIndex(g->nv, i, vert2);

			if ( edge_covered[edge] )
			{
				/* already found this in a perfect matching! */
				continue;
			}

			/* clear labels */
			for ( int i = 0; i < g->nv; i++ )
			{
				labels[i] = -1;
			}

			/* remove j */
			int vj = g->v[vert2];

			g->v[vert2] = -1;

			bool sub_result = hasPM(g, labels);

			/* replace j */
			g->v[vert2] = vj;

			if ( sub_result == false )
			{
				result = false;
				break;
			}

			/* place edges for labels! */
			for ( int k = 0; k < g->nv && labels[k] >= 0; k++ )
			{
				edge_covered[labels[k]] = true;
			}
		}

		/* reset i */
		g->v[i] = vi;

		if ( result == false )
		{
			break;
		}

	}

	free(labels);
	free(edge_covered);

	return result;
}

int fillBarriersToMax(sparsegraph* g, int num_barriers, int** barriers, int* b_sizes)
{
	bool all_complete = true;
	int max_empty_size = 0;

	for ( int i = 0; i < num_barriers; i++ )
	{
		if ( b_sizes[i] > 1 )
		{
			/* only need to test the first pair in the barrier */
			int vj = barriers[i][0];
			int vk = barriers[i][b_sizes[i] - 1];

			/* if the barrier is complete, the LAST edge is pointing to the last
			 * 	barrier element
			 */
			bool are_adjacent = false;

			for ( int k = 0; k < g->d[vj]; k++ )
			{
				if ( g->e[g->v[vj] + k] == vk )
				{
					/* this barrier is already complete! */

					are_adjacent = true;
					break;
				}
			}

			if ( !are_adjacent )
			{
				all_complete = false;

				if ( b_sizes[i] > max_empty_size )
				{
					max_empty_size = b_sizes[i];
				}
			}
		}
	}

	if ( all_complete )
	{
		return g->nde / 2;
	}

	int result = 0;

	/* otherwise, we need to fill one in! */
	for ( int i = 0; i < num_barriers; i++ )
	{
		if ( b_sizes[i] == max_empty_size )
		{
			/* only need to test the first and last elements in the barrier */
			int vj = barriers[i][0];
			int vk = barriers[i][1];

			bool are_adjacent = false;

			for ( int k = 0; k < g->d[vj]; k++ )
			{
				if ( g->e[g->v[vj] + k] == vk )
				{
					/* this barrier is already complete! */

					are_adjacent = true;
					break;
				}
			}

			if ( are_adjacent )
			{
				continue;
			}

			/* make barrier complete */
			makeBarrierComplete(g, barriers[i], b_sizes[i]);

			/* try filling this one in */
			int new_num_barriers;
			int** new_barriers;
			int* new_b_sizes;

			refineBarriers(g, &new_num_barriers, &new_barriers, &new_b_sizes);

			/* recurse */
			int par_result = fillBarriersToMax(g, new_num_barriers, new_barriers, new_b_sizes);

			if ( par_result > result )
			{
				result = par_result;
			}

			/* free, and reset graph */
			for ( int j = 0; j < new_num_barriers; j++ )
			{
				free(new_barriers[j]);
			}
			free(new_b_sizes);
			free(new_barriers);

			makeBarrierIndependent(g, barriers[i], b_sizes[i]);
		}
	}

	return result;
}

/**
 * getMaximumElementarySize
 */
int getMaximumElementarySize(sparsegraph* g)
{
	if ( !is1Extendable(g) )
	{
		//		printf("----[getMaximumElementarySize] \t\t Not 1-extendable!\n");
		//		printModifiedGraphBarrier(g);

		/* if not 1-extendable here, then ok BE SURE NOT TO PRUNE! */
		return g->nv * g->nv;
	}

	/* find the list of current barriers */
	int new_num_barriers;
	int** new_barriers;
	int* new_b_sizes;

	refineBarriers(g, &new_num_barriers, &new_barriers, &new_b_sizes);

	/* recurse */
	int result = fillBarriersToMax(g, new_num_barriers, new_barriers, new_b_sizes);

	/* free, and reset graph */
	for ( int j = 0; j < new_num_barriers; j++ )
	{
		free(new_barriers[j]);
	}

	free(new_barriers);
	free(new_b_sizes);

	return result;
}
