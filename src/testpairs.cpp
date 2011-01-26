/*
 * testpairs.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include "pairs.h"

int main(void)
{
	int i, j, n;

	n = 10000;

	for ( i = 0; i < n; i++ )
	{
		for ( j = 0; j < n; j++ )
		{
			if ( i != j )
			{
				int ii, jj, index;

				index = pairToIndex(n, i, j);

				indexToPair(n, index, ii, jj);

				if ( (ii == i && jj == j) || (jj == i && ii == j) )
				{

				}
				else
				{
					printf("%3d %3d -> %6d -> %3d %3d.\n", i, j, index, ii, jj);
				}
			}
		}
	}

	return 0;
}
