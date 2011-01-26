/*
 * flows.cpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include <string.h>
#include "flows.hpp"

/**
 * findPath
 *
 * Use DFS to find a path in the residual network.
 */
int* findPath(sparsegraph* g, int v1, int v2, int* v_flows, int* v_caps,
		int* e_flows, int* e_caps, int* cur_path, int cur_path_len)
{
	if ( cur_path == 0 || cur_path_len == 0 )
	{
		cur_path = (int*) malloc(g->nv * sizeof(int));
		cur_path_len = 1;

		int* foundPath = findPath(g, v1, v2, v_flows, v_caps, e_flows, e_caps,
				cur_path, cur_path_len);

		free(cur_path);
		return foundPath;
	}

	int cur_vert = cur_path[cur_path_len-1];

	/* search over all neighbors for a vertex with open capacity! */
}

/**
 * achiveFlow
 *
 * Does the given graph achieve the requested flow without the specified ear?
 *
 * @param g an undirected sparsegraph
 * @param v1 an index of a vertex
 * @param v2 an index of another vertex
 * @param without_ear an array of vertices, -1 terminated.
 * @param max_flow (just use 2)
 */
bool achieveFlow(sparsegraph* g, int v1, int v2, int* without_ear, int max_flow)
{
	int* v_flows = (int*) malloc(g->nv * sizeof(int));
	int* v_caps = (int*) malloc(g->nv * sizeof(int));
	int* e_flows = (int*) malloc(g->nv * g->nv * sizeof(int));
	int* e_caps = (int*) malloc(g->nv * g->nv * sizeof(int));

	bzero(v_flows, g->nv * sizeof(int));
	bzero(e_flows, g->nv * g->nv * sizeof(int));

	for ( int i = 0; i < g->nv; i++ )
	{
		v_caps[i] = 1;
	}

	for ( int i = 0; i < g->nv * g->nv; i++ )
	{
		e_caps[i] = 1;
	}

	/* lower capacity on these verts */
	for ( int i = 1; without_ear[i] != v2 && without_ear[i] != v1; i++ )
	{
		v_caps[i] = 0;
	}

	int total_cap = 0;

	while ( total_cap < max_flow )
	{
		/* attempt to increase the total_cap */
		/* try to find a path in the residual network */

	}
}
