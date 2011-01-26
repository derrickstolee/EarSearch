/*
 * deletiontester.cpp
 *
 * Take an input graph, and then delete the gra
 *
 *  Created on: Nov 10, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nausparse.h"
#include "symmetry.h"
#include "pairs.h"
#include "gtools.h"
#include "MatchingDeleter.hpp"
#include "EnumerateDeleter.hpp"
#include "BarrierAlgorithm.hpp"

void printModifiedGraph(sparsegraph* graph)
{
	/* print, but convert into normal graph first.. */
	sparsegraph sg;
	SG_INIT(sg);

	sg.w = 0;
	sg.wlen = 0;

	sg.v = (int*) malloc(graph->nv * sizeof(int));
	sg.d = (int*) malloc(graph->nv * sizeof(int));
	sg.e = (int*) malloc(graph->nde * sizeof(int));

	int iindex = 0;
	int vindex = 0;
	for ( int i = 0; i < graph->nv; i++ )
	{
		/* if vertex is not deleted */
		if ( graph->v[i] >= 0 )
		{
			sg.v[iindex] = vindex;
			sg.d[iindex] = 0;

			for ( int j = 0; j < graph->d[i]; j++ )
			{
				/* if edge is not deleted */
				int edge = graph->e[graph->v[i] + j];
				if ( edge >= 0 && graph->v[edge] >= 0 )
				{
					int jindex = 0;
					for ( int k = 0; k < edge; k++ )
					{
						if ( graph->v[k] >= 0 )
						{
							jindex++;
						}
					}

					sg.e[vindex] = jindex;
					sg.d[iindex] = sg.d[iindex] + 1;

					printf("%d -> %d ----> %d -> %d\n", i, edge, iindex, jindex);

					vindex++;
				}
			}

			iindex++;
		}
	}

	sg.nv = iindex;
	sg.dlen = iindex;
	sg.vlen = iindex;

	sg.nde = vindex;
	sg.elen = vindex;

	printf("%s", sgtos6(&sg));

	SG_FREE(sg);
}

int main(int argc, char** argv)
{
	if ( argc < 2 )
	{
		printf("Usage: deletiontester graph\n");
		exit(1);
	}

	EarDeletionAlgorithm* deleter = new EnumerateDeleter();// MatchingDeleter(1, 100, 100);

	sparsegraph g;
	SG_INIT(g);
	int nloops = 0;

	stringtosparsegraph(argv[1], &g, &nloops);

	int orig_n = g.nv;

	sparsegraph* cur_g = &g;

	while ( cur_g->nde > 0 )
	{
		printModifiedGraph(cur_g);

		/* check if cur_g is a cycle */
		bool is_cycle = true;
		int cur_n = 0;

		int* actual_degs = (int*) malloc(orig_n * sizeof(int));

		for ( int i = 0; i < orig_n; i++ )
		{
			if ( cur_g->v[i] >= 0 )
			{
				cur_n++;
				int actual_deg = 0;

				for ( int j = 0; j < cur_g->d[i]; j++ )
				{
					int edge = cur_g->e[cur_g->v[i] + j];
					if ( edge >= 0 && cur_g->v[edge] >= 0 )
					{
						actual_deg++;
					}
				}

				if ( actual_deg > 2 )
				{
					is_cycle = false;
				}

				actual_degs[i] = actual_deg;
			}
			else
			{
				actual_degs[i] = 0;
			}
		}

		/* compute ears at cur_g */
		int num_ears = 0;
		char **ear_list = (char**) malloc(cur_g->nde * sizeof(char*));

		int* ear_map = (int*) malloc(orig_n * sizeof(int));

		if ( !is_cycle )
		{
			for ( int i = 0; i < orig_n; i++ )
			{
				ear_map[i] = -1;
			}

			for ( int i = 0; i < orig_n; i++ )
			{
				if ( cur_g->v[i] >= 0 && actual_degs[i] > 2 )
				{
					/* we are at the endpoint of some ears! */

					for ( int j = 0; j < cur_g->d[i]; j++ )
					{
						int edge = cur_g->e[cur_g->v[i] + j];
						if ( edge >= 0 && cur_g->v[edge] >= 0 )
						{
							/* the edge is there... and the vertex! */

							if ( actual_degs[edge] == 2 )
							{
								/* this must be a non-trivial ear! */
								/* follow it! */
								ear_list[num_ears] = (char*) malloc(cur_g->nv
										* sizeof(char));

								ear_list[num_ears][0] = i;

								printf("--new ear[%d] = %d ", num_ears, i);

								int ear_l = 1;
								int back_v = i;
								int v = edge;
								while ( actual_degs[v] == 2 )
								{
									printf("%d ", v);
									ear_list[num_ears][ear_l] = v;
									ear_l++;
									ear_map[v] = num_ears;

									/* move to the next spot */
									for ( int k = 0; k < cur_g->d[v]; k++ )
									{
										int kedge = cur_g->e[cur_g->v[v] + k];
										if ( kedge >= 0 && cur_g->v[kedge] >= 0
												&& kedge != back_v )
										{
											back_v = v;
											v = kedge;
											break;
										}
									}
								}

								ear_list[num_ears][ear_l] = v;

								printf("%d ", v);

								if ( v < i )
								{
									printf("X DUPLICATE\n");
									free(ear_list[num_ears]);
								}
								else
								{
									printf("\n");
									ear_list[num_ears][ear_l + 1] = -1;
									num_ears++;
								}
							}
							else if ( edge > i )
							{
								/* to count this edge exactly once, use it in increasing direction */
								ear_list[num_ears] = (char*) malloc(3
										* sizeof(char));
								ear_list[num_ears][0] = i;
								ear_list[num_ears][1] = edge;
								ear_list[num_ears][2] = -1;

								printf("--new ear[%d] = %d %d \n", num_ears, i,
										edge);
								num_ears++;
							}
						} /* if neighbor/edge isn't deleted */
					} /* for all neighbors */
				} /* if not deleted and degree > 2 */
			}
		}

		printf("\t\t\t-- the graph = \n");

		int cur_i = 0;
		for ( int i = 0; i < orig_n; i++ )
		{
			if ( cur_g->v[i] >= 0 )
			{
				printf("\t\t\t %d (%d) -> ", i, cur_i);

				for ( int j = 0; j < cur_g->d[i]; j++ )
				{
					int edge = cur_g->e[cur_g->v[i] + j];

					if ( edge >= 0 && cur_g->v[edge] >= 0 )
					{
						printf("%d ", edge);
					}
				}
				printf("\n");

				cur_i++;
			}
		}

		if ( is1Extendable(cur_g) )
		{
			printf("\t\t\t\t\tis 1-extendable.\n");
		}
		else
		{
			printf("\t\t\t\t\tis NOT 1-extendable??????? \n");
		}

		/* compute symmetry at cur_g */
		int numPairOrbits = 0;
		int** orbitList = (int**) malloc(cur_g->nv * cur_g->nv * sizeof(int*));
		int* canonicalLabels = (int*) malloc(cur_g->nv * sizeof(int));

		numPairOrbits = getCanonicalLabelsAndPairOrbits(cur_g, canonicalLabels,
				orbitList);

		for ( int i = 0; i < numPairOrbits; i++ )
		{
			printf("\t\t--pair orbit %d = ", i);

			for ( int j = 0; orbitList[i][j] >= 0; j++ )
			{
				int vi, vj;
				indexToPair(orig_n, orbitList[i][j], vi, vj);

				printf("(%d;%d,%d) ", orbitList[i][j], vi, vj);
			}
			printf("\n");
		}

		if ( is_cycle )
		{
			printf("--this is a cycle!\n");

			printf("\t\t\t-- the graph = \n");

			int cur_i = 0;
			for ( int i = 0; i < orig_n; i++ )
			{
				if ( cur_g->v[i] >= 0 )
				{
					printf("\t\t\t %d (%d) -> ", i, cur_i);

					for ( int j = 0; j < cur_g->d[i]; j++ )
					{
						int edge = cur_g->e[cur_g->v[i] + j];

						if ( edge >= 0 && cur_g->v[edge] >= 0 )
						{
							printf("%d ", edge);
						}
					}
					printf("\n");

					cur_i++;
				}
			}

			return 0;
		}

		/* next, check canonical deletion */
		int canon_ear = 0; //

		/* TODO: re-implement with the NEW interface! */
		// deleter->getCanonical(cur_g, canonicalLabels, num_ears,	ear_list);

		if ( canon_ear < 0 )
		{
			/* deleter failed! */
			printf("The deleter gave a negative orbit...\n");
			return 1;
		}

		/*  check orbits of ears! */
		int orb1 = ear_list[canon_ear][0];

		int orb2 = 0;
		int i = 1;
		int ear_length;
		for ( i = 1; ear_list[canon_ear][i] >= 0; i++ )
		{
			orb2 = ear_list[canon_ear][i];
		}

		ear_length = i - 2;
		char* ear = ear_list[canon_ear];

		/* get the pair index for the ear that was added */
		int epair = pairToIndex(cur_g->nv, ear[0], ear[ear_length + 1]);

		/* get the pair index for the orbit which was selected as canonical */
		int opair = pairToIndex(cur_g->nv, orb1, orb2);

		printf("--deleting ear of length %d between %d and %d.\n", ear_length,
				orb1, orb2);

		/* delete ear from cur_g */
		if ( ear_length == 0 )
		{
			int ce_v1 = ear[0];
			int ce_v2 = ear[ear_length + 1];

			/* need to delete the EDGE */
			/* the ear is ONE edge */
			/* need to delete just the edge */
			int e1pos = -1;
			int e2pos = -1;
			for ( int j = 0; j < cur_g->d[ce_v1]; j++ )
			{
				if ( cur_g->e[cur_g->v[ce_v1] + j] == ce_v2 )
				{
					e1pos = cur_g->v[ce_v1] + j;
					break;
				}
			}

			for ( int j = 0; j < cur_g->d[ce_v2]; j++ )
			{
				if ( cur_g->e[cur_g->v[ce_v2] + j] == ce_v1 )
				{
					e2pos = cur_g->v[ce_v2] + j;
					break;
				}
			}

			if ( e1pos < 0 || e2pos < 0 )
			{
				printf("-- E1POS = %d AND E2POS = %d.\n", e1pos, e2pos);
				exit(1);
			}
			else
			{
				/* delete the edge */
				cur_g->e[e1pos] = -1;
				cur_g->e[e2pos] = -1;
			}

			cur_g->nde = cur_g->nde - 2;
		}
		else
		{
			printf("-- deleting vertices ");
			for ( int i = 1; i <= ear_length; i++ )
			{
				printf("%d ", ear[i]);
				cur_g->v[ear[i]] = -1;
			}
			printf("\n");

			cur_g->nde = cur_g->nde - 2 * (ear_length + 1);
			// cur_g->nv = cur_g->nv - ear_length;
			/* don't do this, becuase they are not necessary consecutive! */
		}

		/* cleanup */
		for ( int i = 0; i < num_ears; i++ )
		{
			free(ear_list[i]);
		}
		free(ear_list);

		for ( int i = 0; i < numPairOrbits; i++ )
		{
			free(orbitList[i]);
		}
		free(orbitList);
		free(canonicalLabels);

	} /* end while */

	return 0;
}
