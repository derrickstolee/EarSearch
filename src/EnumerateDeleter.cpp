/*
 * EnumerateDeleter.cpp
 *
 *  Created on: Dec 18, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include "EnumerateDeleter.hpp"
#include "connectivity.hpp"

/**
 * Constructor
 */
EnumerateDeleter::EnumerateDeleter()
{

}

/**
 * Destructor
 */
EnumerateDeleter::~EnumerateDeleter()
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
int EnumerateDeleter::getCanonical(EarNode* parent, EarNode* child)
{
	sparsegraph* graph = child->graph;
	int* labels = child->canonicalLabels;
	int num_ears = child->num_ears;
	char** ears = child->ear_list;
	/* return shortest ear with smallest lex label */
	/* However, we must only delete ears which leave G 2-connected! */

	int min_ear_length = graph->nv + 5;
	int min_ear_index = -1;
	int min_ear_label1 = graph->nv;
	int min_ear_label2 = graph->nv;

	for ( int i = 0; i < num_ears; i++ )
	{
		/* find ear length */
		int len = 0;
		for ( ; ears[i][len] >= 0; len++ )
		{

		}

		int ear_lab_1 = labels[ears[i][0]];
		int ear_lab_2 = labels[ears[i][len - 1]];

		if ( ear_lab_1 > ear_lab_2 )
		{
			/* swap */
			int t = ear_lab_1;
			ear_lab_1 = ear_lab_2;
			ear_lab_2 = t;
		}
		/* check that without this ear, we are two-connected! */
		/* i.e., flow between these verts WITHOUT the edge/internal verts is at least 2 */

		if ( len <= min_ear_length )
		{
			if ( is2connected(graph, ears[i][0], ears[i][len - 1], ears[i]) )
			{
				if ( len < min_ear_length )
				{
					min_ear_length = len;
					min_ear_label1 = ear_lab_1;
					min_ear_label2 = ear_lab_2;
					min_ear_index = i;
				}
				else if ( ear_lab_1 < min_ear_label1 || ((ear_lab_1
						== min_ear_label1) && (ear_lab_2 < min_ear_label2)) )
				{
					min_ear_length = len;
					min_ear_label1 = ear_lab_1;
					min_ear_label2 = ear_lab_2;
					min_ear_index = i;
				}
			}
		}
	}

	return min_ear_index;
}
