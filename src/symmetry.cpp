/*
 * symmetry.cpp
 *
 *  Created on: Oct 29, 2010
 *      Author: derrickstolee
 */

#include "pairs.h"
#include "nausparse.h"
#include "nauty.h"
#include "gtools.h"
#include "symmetry.h"

int root(int i, int* p1)
{
	int j = p1[i];

	if ( j == i )
	{
		return j;
	}
	else
	{
		p1[i] = root(j, p1);
		return p1[i];
	}
}

/**
 * Take the two partitions and join them, modifying p1.
 */
void joinPartitions2(int* p1, int* p2, int ne)
{
	bool changed = true;
	while ( changed )
	{
		changed = false;

		for ( int i = 0; i < ne; i++ )
		{
			int rooti = root(i, p1);

			if ( p2[i] == i )
			{
				/* look through a 2-part */
				for ( int j = i; j < ne; j++ )
				{
					int rootj = root(j,p1);

					if ( p2[j] == i && rooti != rootj )
					{
						/* merge the i-part and j-parts */
						if( rooti < rootj )
						{
							p1[rootj] = rooti;
						}
						else if ( rootj < rooti )
						{
							p1[rooti] = rootj;
						}

						changed = true;
					}
				}
			}
		}
	} /* while changed */
}

int gen_n = 0;
int nchoose2 = 0;
int* pair_orbits = 0;

void init_generator_data(int n)
{
	gen_n = n;
	nchoose2 = (n * (n - 1)) / 2;
	pair_orbits = (int*) malloc(nchoose2 * sizeof(int));
//	printf("NEW NAUTY CALL.\n");

	for ( int i = 0; i < nchoose2; i++ )
	{
		pair_orbits[i] = i;
	}
}

void use_generator(int count, permutation *perm, int *orbits, int numorbits,
		int stabvertex, int n)
{
//	printf("\tgenerator ");
//	for ( int i = 0; i < n; i++ )
//	{
//		printf("%d ", perm[i]);
//	}
//	printf("\n");

	int* new_partition = (int*) malloc(nchoose2 * sizeof(int));

	for ( int i = 0; i < nchoose2; i++ )
	{
		new_partition[i] = nchoose2 + 1;
	}

	for ( int start_pindex = 0; start_pindex < nchoose2; start_pindex++ )
	{
		int i, j;
		indexToPair(gen_n, start_pindex, i, j);

		if ( new_partition[start_pindex] >= nchoose2 )
		{
			/* loop with generator! */
			new_partition[start_pindex] = start_pindex;

			int ni = perm[i];
			int nj = perm[j];

			int pindex = pairToIndex(gen_n, ni, nj);

			/* wait until ordered pair match */
			while ( ni != i || nj != j )
			{
				new_partition[pindex] = start_pindex;
				ni = perm[ni];
				nj = perm[nj];
				pindex = pairToIndex(gen_n, ni, nj);
			}
		}
	}

	joinPartitions2(pair_orbits, new_partition, nchoose2);

	free(new_partition);
}

void clean_generator_data()
{
	nchoose2 = 0;
	if ( pair_orbits != 0 )
	{
		free(pair_orbits);
		pair_orbits = 0;
	}
}

int getCanonicalLabelsAndPairOrbits(sparsegraph* g, int* labels,
		int** orbitList)
{
	/* now, compute new orbits! */
//	printf("\t the graph -- %s", (char*) (sgtos6(g) + 1));

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

	free(down_verts);

	sg.nv = vindex;
	sg.dlen = vindex;
	sg.vlen = vindex;

	sg.nde = eindex;
	sg.elen = eindex;

	int nv = sg.nv;
	int m = (nv + WORDSIZE - 1) / WORDSIZE;
	nauty_check(WORDSIZE, m, nv, NAUTYVERSIONID);

	DYNALLSTAT(int, lab, lab_n);
	DYNALLSTAT(int, ptn, ptn_n);
	DYNALLSTAT(int, orbits, orbits_n);

	DYNALLOC1(int, lab, lab_n, nv, "malloc");
	DYNALLOC1(int, ptn, ptn_n, nv, "malloc");
	DYNALLOC1(int, orbits, orbits_n, nv, "malloc");

	for ( int i = 0; i < nv; i++ )
	{
		lab[i] = i;
		ptn[i] = 0;
	}

	static DEFAULTOPTIONS_SPARSEGRAPH( options);

	init_generator_data(sg.nv);

	options.defaultptn = TRUE; /* don't need colors */
	options.getcanon = TRUE; /* gets labels */
	options.digraph = FALSE;
	options.userautomproc = &use_generator;

	options.invarproc = &adjacencies_sg; /* sparse version */

	statsblk stats; /* we'll use this at the end */
	DYNALLSTAT(setword, workspace, worksize);
	DYNALLOC1(setword, workspace, worksize, 50 * m, "malloc");

	sparsegraph canon_g;
	SG_INIT(canon_g);

	/* call nauty */
	clock_t start_c = clock();
	time_t start_t = time(NULL);
	nauty((graph*) &sg, lab, ptn, NULL, orbits, &options, &stats, workspace, 50
			* m, m, sg.nv, (graph*) &canon_g);
	clock_t end_c = clock();
	time_t end_t = time(NULL);

	/* todo: record nauty time */

	for ( int i = 0; i < g->nv; i++ )
	{
		/* "blank" is a big label */
		labels[i] = 2 * g->nv;
	}

	for ( int i = 0; i < sg.nv; i++ )
	{
		/* use labels for g */
		int canon_lab_i = 2 * g->nv;
		for ( int j = 0; j < sg.nv; j++ )
		{
			if ( lab[j] == i )
			{
				canon_lab_i = j;
				break;
			}
		}

		labels[rev_verts[i]] = canon_lab_i;
	}

	int index = 0;
	int* orbitReps = (int*) malloc(g->nv * g->nv * sizeof(int));
	bzero(orbitReps, g->nv * g->nv * sizeof(int));

	for ( int i = 0; i < sg.nv; i++ )
	{
		for ( int j = i + 1; j < sg.nv; j++ )
		{
			int pair_ij = pairToIndex(sg.nv, i, j);

			if ( pair_orbits[pair_ij] == pair_ij )
			{
				/* this is the orbit representative */
				orbitReps[pair_ij] = index;
				orbitList[index] = (int*) malloc(g->nv * g->nv * sizeof(int));

				for ( int k = 0; k < g->nv * g->nv; k++ )
				{
					orbitList[index][k] = -1;
				}

				/* fill orbits with pair from g */
				orbitList[index][0] = pairToIndex(g->nv, rev_verts[i],
						rev_verts[j]);

				int k = 1;
				for ( int pair_kl = pair_ij + 1; pair_kl
						< (sg.nv * (sg.nv - 1)) / 2; pair_kl++ )
				{
					if ( pair_orbits[pair_kl] == pair_ij )
					{
						int kk, ll;
						indexToPair(sg.nv, pair_kl, kk, ll);
						orbitList[index][k] = pairToIndex(g->nv, rev_verts[kk],
								rev_verts[ll]);
						k++;
					}
				}
				orbitList[index][k] = -1;
				orbitList[index][k + 1] = -1;

				/* increase the pair orbit */
				index++;
			}
		}
	}

	free(orbitReps);
	free(rev_verts);

	//	sortlists_sg(&canon_g);
	//	printf("%s", sgtos6(&canon_g));

	/* free workspace */
	DYNFREE(workspace, worksize);
	DYNFREE(lab,lab_n);
	DYNFREE(ptn,ptn_n);
	DYNFREE(orbits,orbits_n);
	SG_FREE(canon_g);
	SG_FREE(sg);
	clean_generator_data();

	return index;
}
