/*
 * MatchingPruner.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include "gtools.h"
#include "MatchingPruner.hpp"
#include "permcount.h"
#include "BarrierAlgorithm.hpp"

/**
 * Constructor
 *
 * @param p the number of perfect matchings we are looking for.
 * @param n the maximum number of vertices allowed.
 */
MatchingPruner::MatchingPruner(int pmin, int pmax, int n, int c)
{
	this->pmin = pmin;
	this->pmax = pmax;
	this->n = n;
	this->c = c;
}

/**
 * Destructor
 */
MatchingPruner::~MatchingPruner()
{

}

/**
 * checkPrune -- check if the given graph needs to be pruned.
 *
 * @return 1 iff the graph should be pruned.
 */
int MatchingPruner::checkPrune(EarNode* parent, EarNode* child)
{
	if ( child->graph == 0 )
	{
		//		printf("--[MatchingPruner] No child graph.\n");
		return 0;
	}

	sparsegraph* cg = child->graph;

	if ( cg->nv % 2 == 1 || cg->nv > this->n )
	{
		/* odd! VERY BAD */
		//		printf("--[MatchingPruner] Not 1-extendable by oddness: %s", sgtos6(cg));
		return 1;
	}

	if ( parent == 0 )
	{
		//		printf("--[MatchingPruner] No parent.\n");
		return 0;
	}

	if ( parent->numMatchings >= this->pmax )
	{
		/* We need to prune, since the parent failed to be a solution! */
		//		printf(
		//				"--[MatchingPruner] Parent had %d perfect matchings which is too many!\n",
		//				parent->numMatchings);
		return 1;
	}

	int cg_matchings = countPM(cg, this->pmax);
	child->numMatchings = cg_matchings;

	if ( cg_matchings == 0 )
	{
		/* this is bad and unallowed */
		//		printf(
		//				"--[MatchingPruner] Not 1-extendable by no perfect matchings: %s",
		//				sgtos6(cg));
		return 1;
	}

	if ( cg_matchings > this->pmax )
	{
		/* this is bad and unallowed */
		//		printf("--[MatchingPruner] Too many perfect matchings (P >= %d): %s",
		//				cg_matchings, sgtos6(cg));
		return 1;
	}

	if ( parent->graph == 0 )
	{
		//		//printf("--[MatchingPruner] No parent graph.\n");
		return 0;
	}

	/* NEW perfect matchings are those with these covered */

	if ( parent->ear == 0 )
	{
		/* parent is a cycle... anything goes! */
		parent->extendable = true;
	}

	if ( child->ear == 0 )
	{
		/* child is a cycle... anything goes! */
		child->extendable = true;

		return 0;
	}

	/* TIME TO CHECK 1-EXTENDABILITY */
	int v1 = child->ear[0];
	int v2 = child->ear[child->ear_length + 1];


	/* If G-uv has perfect matchings, we are 1-extendable... */
	/* the parent graph is the same data */
	/* we mark the ear as being matched and test what remains */
	char* ear = child->ear;
	int length = child->ear_length;

	/* even number of interior points */
	int* earInterior = (int*) malloc((2 + length) * sizeof(int));

	/* delete entire ear */
	for ( int i = 0; i <= length + 1; i++ )
	{
		earInterior[i] = cg->v[ear[i]];
		cg->v[ear[i]] = -1;
	}

	/* NEW perfect matchings are those with these covered */
	bool cg_matchings_without = hasPM(cg);

	/* replace entire ear */
	for ( int i = 0; i <= length + 1; i++ )
	{
		cg->v[ear[i]] = earInterior[i];
	}

	free(earInterior);

	if ( cg_matchings_without == false && cg_matchings >= this->pmax )
	{
		/* we would need to ADD perfect matchings to be 1-extendable! */
		//		printf(
		//				"--[MP] Would need to ADD perfect matchings to be 1-extendable! (no longer 1-ext. and %d perf. matchings. \n",
		//				cg_matchings);
		return 1;
	}

	if ( cg_matchings_without )
	{
		/* we are extendable! */
		child->extendable = true;
	}

	if ( !parent->extendable )
	{
		/* parent is not extendable, so we need a two-ear addition */

		if ( 1 )
		{
			/* check using known ear information */
			char* p_ear = parent->ear;
			int p_length = parent->ear_length;

			/* need to check that the current ear is nowhere inside the parent ear */
			for ( int i = 0; i <= p_length + 1; i++ )
			{
				if ( p_ear[i] == ear[0] || p_ear[i] == ear[length + 1] )
				{
					/* an endpoint of the current ear is in the interior of the parent ear */
					return 1;
				}
			}

			/* deleting the endpoints of the current ear */
			/* even number of interior points */
			int* ear1Interior = (int*) malloc((2 + length) * sizeof(int));

			/* delete entire ear */
			for ( int i = 0; i <= length + 1; i++ )
			{
				ear1Interior[i] = cg->v[ear[i]];
				cg->v[ear[i]] = -1;
			}

			/* also deleting the endpoints of the parent's ear */
			/* even number of interior points */
			int* ear2Interior = (int*) malloc((2 + p_length) * sizeof(int));

			/* delete entire ear */
			for ( int i = 0; i <= p_length + 1; i++ )
			{
				ear2Interior[i] = cg->v[p_ear[i]];
				cg->v[p_ear[i]] = -1;
			}

			/* Does it have a  perfect matching without these ears? */
			cg_matchings_without = hasPM(cg);

			/* replacing the parent ear */
			for ( int i = 0; i <= p_length + 1; i++ )
			{
				cg->v[p_ear[i]] = ear2Interior[i];
			}
			free(ear2Interior);

			/* replacing the current ear */
			for ( int i = 0; i <= length + 1; i++ )
			{
				cg->v[ear[i]] = ear1Interior[i];
			}
			free(ear1Interior);

			if ( cg_matchings_without == false )
			{
				/* the pair is not a two-ear addition */
				//			printf(
				//					"--[MatchingPruner] Pruning since we needed two ears but did not get it.\n");
				return 1;
			}
			else
			{
				/* the pair IS an addition! */
				child->extendable = true;
			}
		}
		else
		{
			/* check using no structural information */
			/* based on tests, this is slightly slower */
			if ( !is1Extendable(cg) )
			{
				/* it's a two-ear addition, but not 1-extendable! */
				return 1;
			}
			else
			{
				child->extendable = true;
			}
		}
	}

	/* prune based on barrier density */
	int max_edges = getMaximumElementarySize(cg);

	if ( max_edges + 2 * (this->pmax - cg_matchings) + ((this->n - cg->nv)/2) < (cg->nv * cg->nv) / 4
			+ this->c )
	{
		return 1;
	}

	/**
	 * The following prune is from the pruning algorithm, following
	 * 	Lemma \ref{lma:pruning} (Currently, 4.4.9)
	 */
	int constant = max_edges - (cg->nv * cg->nv) / 4;

	bool can_reach = false;
	for ( int between_n = cg->nv; between_n <= this->n; between_n += 2 )
	{
		int between_constant = constant + 2*(this->pmax - cg_matchings) - ((between_n-cg->nv)*(cg->nv - 2))/4;
		if ( between_constant >= this->c )
		{
			can_reach = true;
			break;
		}
	}

	if ( !can_reach )
	{
		return 1;
	}

	return 0;
}

