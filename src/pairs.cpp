/*
 * pairs.c
 *
 *  Created on: Oct 28, 2010
 *      Author: derrickstolee
 */

#include "pairs.h"

/*************************************/
/********TRANSLATION METHODS**********/
/*************************************/

/**
 * pairToIndex
 *
 * @param n The total number of vertices.
 * @param v1
 * @param v2
 * @return the index for edge {v1, v2}
 */
int pairToIndex(int n, int v1, int v2)
{
	if ( v1 == v2 )
	{
		return -1;
	}

	if ( v1 > v2 )
	{
		return pairToIndex(n, v2, v1);
	}

	int index = 0;

	for ( int i = 0; i < v1; i++ )
	{
		index += n - (i + 1);
	}

	index += v2 - (v1 + 1);

	return index;
}

/**
 * indexToPair
 *
 * @param n The total number of vertices.
 * @param index
 * @param v1
 * @param v2
 * @return the index for edge {v1, v2}
 */
void indexToPair(int n, int index, int& v1, int& v2)
{
	int index2 = index;
	int i;
	for ( i = 0; index2 >= n - (i + 1); i++ )
	{
		index2 = index2 - (n - (i + 1));
	}

	v1 = i;
	v2 = index2 + i + 1;
}
