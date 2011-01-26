/*
 * permcount.c
 *
 *  Created on: Jul 19, 2010
 *      Author: derrickstolee
 *
 *  Take a list of sparse6 representations, 
 * 		take complements, and count the number
 * 		of perfect matchings.
 *  
 */

#include <stdio.h>
#include <string.h>
#include <nauty.h>
#include <nausparse.h>
#include <nautinv.h>
#include <gtools.h>
#include "permcount.h"
#include "pairs.h"

int getConjC(int p)
{
	int t, dblfact, k;
	int last_dblfact;

	t = 1;
	last_dblfact = 1;
	dblfact = 1;

	while ( dblfact <= p )
	{
		t++;
		last_dblfact = dblfact;
		dblfact *= (2 * t - 1);
	}

	k = p / last_dblfact;

	return (t - 1) * (t - 1) - (t - 1) + k - 1;
}

int countPM(sparsegraph* g, int max_p, int* labels)
{
	/* find first vertex non-negative */
	int i;
	int vert = -1;
	for ( i = 0; i < g->nv; i++ )
	{
		/* if vertex is not deleted */
		if ( g->v[i] >= 0 )
		{
			if ( vert < 0 )
			{
				/* pick first unmatched vertex */
				vert = i;
			}

			if ( g->d[i] == 0 )
			{
				/* THIS IS BAD!  Can't match! */
				return 0;
			}
			else
			{
				int vi = g->v[i];
				int actual_deg = 0;

				for ( int j = 0; j < g->d[i]; j++ )
				{
					int vertj = g->e[vi + j];

					/* if edge and vertex are not deleted */
					if ( vertj >= 0 && g->v[vertj] >= 0 )
					{
						actual_deg++;
						/* only care about degree at least 1 */
						break;
					}
				}

				if ( actual_deg == 0 )
				{
					/* BAD! Can't match this vert! */
					return 0;
				}
			}
		}
	}

	if ( vert < 0 )
	{
		/* oh noes! NO unmatched vertices???? */
		/* this is actually a good thing! */
		/* we found a perfect matching. */
		return 1;
	}

	int vert_pos = g->v[vert];
	int count = 0;

	for ( i = vert_pos; i < vert_pos + g->d[vert]; i++ )
	{
		int vert2 = g->e[i];

		if ( vert2 < 0 )
		{
			/* edge deleted */
			continue;
		}

		if ( g->v[vert2] >= 0 )
		{
			/* vertex is not deleted! */
			int vert2_pos = g->v[vert2];

			/* count with this edge */
			g->v[vert] = -1;
			g->v[vert2] = -1;

			int sub_count = 0;

			if ( labels != 0 )
			{
				sub_count = countPM(g, max_p, &(labels[1]));

				if ( sub_count > 0 )
				{
					labels[0] = pairToIndex(g->nv, vert, vert2);
					count += sub_count;
				}
			}
			else
			{
				sub_count = countPM(g, max_p, 0);
				count += sub_count;
			}

			g->v[vert] = vert_pos;
			g->v[vert2] = vert2_pos;

			if ( count > max_p )
			{
				/* too many already! */
				return count;
			}
		}
	}

	return count;
}

int countPM(sparsegraph* g, int max_p)
{
	return countPM(g, max_p, 0);
}

bool hasPM(sparsegraph* g)
{
	if ( countPM(g, 0) >= 1 )
	{
		return true;
	}

	return false;
}

bool hasPM(sparsegraph* g, int* labels)
{
	if ( countPM(g, 0, labels) >= 1 )
	{
		return true;
	}

	return false;
}
