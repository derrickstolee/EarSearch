/*
 * canonstring.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: derrickstolee
 */

#include "nausparse.h"
#include "nauty.h"
#include "gtools.h"
#include "canonstring.hpp"

/**
 * getCanonString
 *
 * Output a canonical string for the given graph.
 *
 * First, it makes a deep copy of g, using the -1 markers in g->v as deleted vertices
 * 	and -1 markers in g->e as deleted edges.
 *
 * Second, it puts this deep copy into nauty and finds a canonical labeling.
 *
 * This canonical labeling is turned into a string.
 *
 * The string is copied from nauty's standard memory into the heap.
 *
 * The requestor is responsible for free()ing the returned string.
 *
 * This method accepts digraphs as well as undirected graphs.
 */
char* getCanonString(sparsegraph* g)
{
	/* sg will be the deep copy */
	sparsegraph sg;
	SG_INIT(sg);

	/* deep copy g with deletions to sg */
	int num_verts = 0;

	int* vert_index = (int*) malloc(g->nv * sizeof(int));
	for ( int i = 0; i < g->nv; i++ )
	{
		vert_index[i] = num_verts;

		if ( g->v[i] >= 0 )
		{
			num_verts++;
		}
	}

	sg.nv = num_verts;
	sg.v = (size_t*) malloc(num_verts * sizeof(size_t));
	sg.d = (int*) malloc(num_verts * sizeof(int));

	/* over-allocation is ok. */
	sg.e = (int*) malloc(num_verts * num_verts * sizeof(int));

	int e_index = 0;
	int v_index = 0;

	for ( int i = 0; i < g->nv; i++ )
	{
		if ( g->v[i] >= 0 )
		{
			sg.v[v_index] = e_index;

			int deg_i = 0;

			for ( int j = 0; j < g->d[i]; j++ )
			{
				if ( g->e[g->v[i] + j] >= 0 && g->v[g->e[g->v[i] + j]] >= 0 )
				{
					/* edge AND vertex not deleted */
					deg_i++;
					sg.e[e_index] = vert_index[g->e[g->v[i] + j]];
					e_index++;
				}
			}

			sg.d[v_index] = e_index - sg.v[v_index];
			v_index++;
		}
	}

	sg.vlen = num_verts;
	sg.dlen = num_verts;
	sg.elen = e_index;
	sg.nde = e_index;

	free(vert_index);
	vert_index = 0;

	/* find a canonical string for sg */
	int nv = sg.nv;
	int m = (nv + WORDSIZE - 1) / WORDSIZE;
	nauty_check(WORDSIZE, m, nv, NAUTYVERSIONID);

	DYNALLSTAT(int, lab, lab_n);
	DYNALLSTAT(int, ptn, ptn_n);
	DYNALLSTAT(int, orbits, orbits_n);

	DYNALLOC1(int, lab, lab_n, nv, "malloc");
	DYNALLOC1(int, ptn, ptn_n, nv, "malloc");
	DYNALLOC1(int, orbits, orbits_n, nv, "malloc");

	static DEFAULTOPTIONS_SPARSEGRAPH( options);

	options.defaultptn = TRUE; /* Don't need colors */
	options.getcanon = TRUE; /* gets labels */

	/* using this means we accept digraphs as well as undirected graphs! */
	/* it may operate slightly slower than usual, but not noticably */
	options.digraph = TRUE;

	/* options.invarproc = &adjacencies_sg; /* sparse version */
	//	options.invarproc = &adjtriang;

	statsblk stats; /* we'll use this at the end */
	DYNALLSTAT(setword, workspace, worksize);
	DYNALLOC1(setword, workspace, worksize, 50 * m, "malloc");

	sparsegraph canon_g;
	SG_INIT(canon_g);

	/* call nauty */
	nauty((graph*) &sg, lab, ptn, NULL, orbits, &options, &stats, workspace, 50
			* m, m, sg.nv, (graph*) &canon_g);

	/* a necessary step to getting a canonical string */
	sortlists_sg(&canon_g);

	char* canon_str = sgtos6(&canon_g);
	int canon_len = strlen(canon_str);

	char* buffer = (char*) malloc(canon_len + 1);
	strcpy(buffer, canon_str);

	/* free workspace */
	DYNFREE(workspace, worksize);
	DYNFREE(lab,lab_n);
	DYNFREE(ptn,ptn_n);
	DYNFREE(orbits,orbits_n);

	SG_FREE(canon_g);
	SG_FREE(sg);

	return buffer;
}
