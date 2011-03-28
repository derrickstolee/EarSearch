/*
 * bipartite.cpp
 *
 *  Created on: Mar 4, 2011
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include "nausparse.h"
#include "bipartite.hpp"

/**
 * isBipartite
 *
 * Is the given graph bipartite?
 */
bool isBipartite(sparsegraph* g)
{
	int * color = (int*) malloc(g->nv * sizeof(int));
	bzero(color, g->nv * sizeof(int));

	color[0] = 1;
	bool result = true;

	for ( int i = 0; result && i < g->nv; i++ )
	{
		if ( color[i] > 0 && color[i] < 3 )
		{
			/* we need to expand on this color! */
			int vi = g->v[i];
			int di = g->d[i];
			for ( int j = 0; result && j < di; j++ )
			{
				int nj = g->e[vi + j];

				if ( color[nj] == 0 )
				{
					/* give it the opposite color! */
					/* 1 -> 2 */
					/* 2 -> 1 */
					color[nj] = (color[i] % 2) + 1;
				}
				else if ( color[nj] < 3 )
				{
					if ( color[nj] == color[i] )
					{
						/* bad! Adjacent with the same color? */
						result = false;
					}
				}
				/* if color >= 3, it was already expanded, and checked this edge */
			}

			color[i] = color[i] + 2;
		}
	}


	free(color);
	return result;
}
