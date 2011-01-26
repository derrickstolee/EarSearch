/*
 * MatchingDeleter.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include <stdio.h>
#include "nausparse.h"
#include "gtools.h"
#include "symmetry.h"
#include "MatchingDeleter.hpp"
#include "BarrierAlgorithm.hpp"


/**
 * Constructor
 *
 * @param p the number of perfect matchings we are looking for.
 * @param n the maximum number of vertices allowed.
 */
MatchingDeleter::MatchingDeleter(int pmin, int pmax, int n)
{
	this->pmin = pmin;
	this->pmax = pmax;
	this->n = n;
}

/**
 * Destructor
 */
MatchingDeleter::~MatchingDeleter()
{

}


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
int MatchingDeleter::getCanonical(EarNode* parent, EarNode* child)
{
	sparsegraph* g = child->graph;
	int* labels = child->canonicalLabels;
	int num_ears = child->num_ears;
	char** ears = child->ear_list;


	if ( (g->nv % 2) == 1 )
	{
		/* odd number of vertices? skip! */
		return -1;
	}

	int bestEar = -1;
	int bestLen = g->nv;
	int label1 = 3 * g->nv;
	int label2 = 3 * g->nv;

	/* see if we can be 1-extendable through a 1-ear deletion */
	for ( int i = 0; i < num_ears; i++ )
	{
		char* cur_ear = ears[i];

		const int ce_v1 = cur_ear[0];

		int ear_verts = 0;
		for ( ; cur_ear[ear_verts] >= 0; ear_verts++ )
		{
		}

		const int ce_len = ear_verts - 2;

		if ( (ce_len % 2) == 1 )
		{
			/* odd ears are bad! */
			continue;
		}

		const int ce_v2 = cur_ear[ce_len + 1];

		/* test if better than the current-best canonical ear! */
		int ce_lab1 = labels[ce_v1];
		int ce_lab2 = labels[ce_v2];

		if ( ce_lab1 > ce_lab2 )
		{
			/* swap! */
			int temp = ce_lab1;
			ce_lab1 = ce_lab2;
			ce_lab2 = temp;
		}

		if ( ce_lab1 > label1 || (ce_lab1 == label1 && ce_lab2 > label2) )
		{
			/* this cannot be canonical, so why care? */
			continue;
		}

		if ( ce_lab1 == label1 && ce_lab2 == label2 && ce_len >= bestLen )
		{
			/* either this cannot be canonical by length, or is already isomorphic to the canonical ear */
			continue;
		}


		bool is1ext = false;

		if ( ce_len > 0 )
		{
			/* erase the internal vertices from g */
			int* internal_V_values = (int*) malloc((ce_len + 2) * sizeof(int));

			for ( int j = 1; j <= ce_len; j++ )
			{
				internal_V_values[j] = g->v[cur_ear[j]];
				g->v[cur_ear[j]] = -1;
			}

			/*  test if cg is now 1-extendable */
			is1ext = is1Extendable(g);

			/* reset graph */
			for ( int j = 1; j <= ce_len; j++ )
			{
				g->v[cur_ear[j]] = internal_V_values[j];
			}
			free(internal_V_values);
		}
		else if ( ce_len == 0 )
		{
			/* the ear is ONE edge */
			/* need to delete just the edge */

			int e1pos = -1;
			int e2pos = -1;
			for ( int j = 0; j < g->d[ce_v1]; j++ )
			{
				if ( g->e[g->v[ce_v1] + j] == ce_v2 )
				{
					e1pos = g->v[ce_v1] + j;
					break;
				}
			}

			for ( int j = 0; j < g->d[ce_v2]; j++ )
			{
				if ( g->e[g->v[ce_v2] + j] == ce_v1 )
				{
					e2pos = g->v[ce_v2] + j;
					break;
				}
			}

			if ( e1pos < 0 || e2pos < 0 )
			{
				printf(
						"--[MatchingDeleter] BAD E1POS = %d OR E2POS = %d in SINGLE DELETION CHECK.\n",
						e1pos, e2pos);
			}
			else
			{
				/* delete the edge */
				g->e[e1pos] = -1;
				g->e[e2pos] = -1;

				/*  test if cg is now 1-extendable */
				is1ext = is1Extendable(g);

				/* replace the edge */
				g->e[e1pos] = ce_v2;
				g->e[e2pos] = ce_v1;
			}
		}

		/* what to do if 1extendable? */
		if ( is1ext )
		{
			/* this is the canonically-best so far! */
			bestEar = i;
			bestLen = ce_len;
			label1 = ce_lab1;
			label2 = ce_lab2;
		}
	}

	if ( label1 < g->nv && label2 < g->nv )
	{
		/* we found a 1-ear deletion to be canonical! */
		return bestEar;
	}

	/* now, we test for a two-ear deletion that is canonical */

	/* For all [even] ears e */
	for ( int i = 0; i < num_ears; i++ )
	{
		char* cur_ear = ears[i];

		const int ce_v1 = cur_ear[0];

		int ear_verts = 0;
		for ( ; cur_ear[ear_verts] >= 0; ear_verts++ )
		{
		}

		int ce_len = ear_verts - 2;
		const int ce_v2 = cur_ear[ce_len + 1];

		if ( (ce_len % 2) == 1 )
		{
			/* odd ears are bad! */
			continue;
		}

		/* test if better than the current-best canonical ear! */
		int ce_lab1 = labels[ce_v1];
		int ce_lab2 = labels[ce_v2];

		if ( ce_lab1 > ce_lab2 )
		{
			/* swap! */
			int temp = ce_lab1;
			ce_lab1 = ce_lab2;
			ce_lab2 = temp;
		}

		if ( ce_lab1 > label1 || (ce_lab1 == label1 && ce_lab2 > label2) )
		{
			/* this cannot be canonical, so why care? */
			continue;
		}

		if ( ce_lab1 == label1 && ce_lab2 == label2 && ce_len >= bestLen )
		{
			/* either this cannot be canonical, or is already isomorphic to the canonical ear */
			continue;
		}

		/* remove e, depending on length */
		bool is2ear = false;
		int* internal_V_values = 0;
		int e1pos = -1, e2pos = -1;

		if ( ce_len > 0 )
		{
			/* erase the internal vertices from g */
			internal_V_values = (int*) malloc((ce_len + 2) * sizeof(int));

			for ( int j = 1; j <= ce_len; j++ )
			{
				internal_V_values[j] = g->v[cur_ear[j]];
				g->v[cur_ear[j]] = -1;
			}
		}
		else if ( ce_len == 0 )
		{
			/* the ear is ONE edge */
			/* need to delete just the edge */
			e1pos = -1;
			e2pos = -1;
			for ( int j = 0; j < g->d[ce_v1]; j++ )
			{
				if ( g->e[g->v[ce_v1] + j] == ce_v2 )
				{
					e1pos = g->v[ce_v1] + j;
					break;
				}
			}

			for ( int j = 0; j < g->d[ce_v2]; j++ )
			{
				if ( g->e[g->v[ce_v2] + j] == ce_v1 )
				{
					e2pos = g->v[ce_v2] + j;
					break;
				}
			}

			if ( e1pos < 0 || e2pos < 0 )
			{
				printf("--[MatchingDeleter] E1POS = %d AND E2POS = %d.\n",
						e1pos, e2pos);
			}
			else
			{
				/* delete the edge */
				g->e[e1pos] = -1;
				g->e[e2pos] = -1;
			}
		}

		/* See if a 2-ear-deletion is possible! */
		for ( int fi = 0; fi < num_ears; fi++ )
		{
			int f_v1 = ears[fi][0];
			int f_v2 = -1;

			int fj = 0;

			/* check if the fi-ear intersects the current ear */
			bool internal_intersect = false;
			for ( fj = 0; ears[fi][fj] >= 0; fj++ )
			{
				f_v2 = ears[fi][fj];

				if ( f_v2 == ce_v1 || f_v2 == ce_v2 )
				{
					internal_intersect = true;
					break;
				}
			}

			if ( internal_intersect )
			{
				/* this ear won't work due to intersection */
				continue;
			}

			int f_len = fj - 2;

			int* internal_F_values = 0;
			int f1pos = -1, f2pos = -1;

			if ( f_len > 0 )
			{
				/* erase the internal vertices from g */
				internal_F_values = (int*) malloc((f_len + 2) * sizeof(int));

				for ( int j = 1; j <= f_len; j++ )
				{
					internal_F_values[j] = g->v[ears[fi][j]];
					g->v[ears[fi][j]] = -1;
				}
			}
			else if ( f_len == 0 )
			{
				/* the ear is ONE edge */
				/* need to delete just the edge */

				f1pos = -1;
				f2pos = -1;
				for ( int j = 0; j < g->d[f_v1]; j++ )
				{
					if ( g->e[g->v[f_v1] + j] == f_v2 )
					{
						f1pos = g->v[f_v1] + j;
						break;
					}
				}

				for ( int j = 0; j < g->d[f_v2]; j++ )
				{
					if ( g->e[g->v[f_v2] + j] == f_v1 )
					{
						f2pos = g->v[f_v2] + j;
						break;
					}
				}

				if ( f1pos < 0 || f2pos < 0 )
				{
					printf("--[MatchingDeleter] F1POS = %d AND F2POS = %d.\n",
							f1pos, f2pos);
				}
				else
				{
					/* delete the edge */
					g->e[f1pos] = -1;
					g->e[f2pos] = -1;
				}
			}

			is2ear = is1Extendable(g);

			/* replace the ear f */
			if ( f_len > 0 )
			{
				for ( int j = 1; j <= f_len; j++ )
				{
					g->v[ears[fi][j]] = internal_F_values[j];
				}

				free(internal_F_values);
			}
			else if ( f_len == 0 && f1pos >= 0 && f2pos >= 0 )
			{
				g->e[f1pos] = f_v2;
				g->e[f2pos] = f_v1;
			}

			if ( is2ear )
			{
				/* check if this is the CANONICAL deletion at this point */
				break;
			}
		} /* end for all other ears */

		/* reset graph in each case */
		if ( ce_len > 0 )
		{
			/* replace the edge e */
			for ( int j = 1; j <= ce_len; j++ )
			{
				g->v[cur_ear[j]] = internal_V_values[j];
			}
			free(internal_V_values);
		}
		else if ( ce_len == 0 && e1pos >= 0 && e2pos >= 0  )
		{
			/* replace the edge e */
			g->e[e1pos] = ce_v2;
			g->e[e2pos] = ce_v1;
		}

		if ( is2ear )
		{
			/* replace canonical label with this one! */
			bestEar = i;
			bestLen = ce_len;
			label1 = ce_lab1;
			label2 = ce_lab2;
		}
	}

	if ( label1 < g->nv && label2 < g->nv )
	{
		//		/* we found a 1-ear deletion to be canonical! */
		return bestEar;
	}
	else
	{
				printf("--[MatchingDeleter] FAILED TO FIND A 2-ear-deletion! %s",
						sgtos6(g));
	}

	return -1;
}
