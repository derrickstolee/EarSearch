/*
 * EarSearchManager.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "canonstring.hpp"
#include "pairs.h"
#include "nausparse.h"
#include "nauty.h"
#include "gtools.h"
#include "symmetry.h"
#include "EarNode.hpp"
#include "EarSearchManager.hpp"
#include "Set.hpp"
#include "TreeSet.hpp"

void printGraphInfo(sparsegraph* g)
{
	//	return;
	//
	//	printf("-- graph\t nv: %d nde: %d\n", g->nv, g->nde);
	//
	//	for ( int i = 0; i < g->nv; i++ )
	//	{
	//		printf("--vert\t %d:\t v: %d \t d: %d \t", i, g->v[i], g->d[i]);
	//
	//		for ( int j = 0; j < g->d[i]; j++ )
	//		{
	//			printf("%d ", g->e[g->v[i]+j]);
	//		}
	//		printf("\n");
	//	}
}

LONG_T getLabel(int length, int orbit)
{
	LONG_T label = (((LONG_T) length) << 16) | (LONG_T) orbit;

	return label;
}

void getInfoFromLabel(LONG_T label, int &length, int &orbit)
{
	length = (int) (label >> 16);
	orbit = (int) (label & 0xFF);
}

bool areAdjacent(sparsegraph* g, int v1, int v2)
{
	if ( g->v[v1] < 0 || g->v[v2] < 0 )
	{
		return false;
	}

	for ( int i = 0; i < g->d[v1]; i++ )
	{
		if ( g->e[g->v[v1] + i] == v2 )
		{
			return true;
		}
	}

	return false;
}

/**
 * constructor
 *
 * @param g a graph to build the data from.
 * 	A deep copy of g is made.
 */
GraphData::GraphData(sparsegraph* g)
{
	this->n = g->nv;
	this->e = g->nde / 2; /* UNDIRECTED edges! */
	this->deg_seq = 0;
	this->invariant = -1;
	this->canon_strings = 0;

	this->g = (sparsegraph*) malloc(sizeof(sparsegraph));
	SG_INIT(*(this->g));

	/* perform a deep copy of g */
	this->g->nv = g->nv;
	this->g->nde = g->nde;
	this->g->vlen = g->nv;
	this->g->dlen = g->nv;
	this->g->elen = g->nde;

	this->g->v = (size_t*) malloc(g->nv * sizeof(size_t));
	this->g->d = (int*) malloc(g->nv * sizeof(int));
	this->g->e = (int*) malloc(g->nde * sizeof(int));

	int v_index = 0;
	for ( int i = 0; i < g->nv; i++ )
	{
		this->g->v[i] = v_index;
		this->g->d[i] = g->d[i];

		for ( int j = 0; j < g->d[i]; j++ )
		{
			this->g->e[v_index] = g->e[g->v[i] + j];
			v_index++;
		}
	}
}

/**
 * Destructor
 */
GraphData::~GraphData()
{
	if ( this->canon_strings != 0 )
	{
		for ( int i = 0; i < this->e; i++ )
		{
			if ( this->canon_strings[i] != 0 )
			{
				free(this->canon_strings[i]);
				this->canon_strings[i] = 0;
			}
		}

		free(this->canon_strings);
		this->canon_strings = 0;
	}

	if ( this->g != 0 )
	{
		SG_FREE(*(this->g));
		free(this->g);
		this->g = 0;
	}

	if ( this->deg_seq != 0 )
	{
		free(this->deg_seq);
		this->deg_seq = 0;
	}

	this->n = 0;
}

/**
 * compare
 *
 * Compare the current graph data to another graph data.
 *
 * @return an integer signifying the progress made.
 * 	-1 means they are the same!
 *   0 means they failed the vertex check.
 *   1 means they failed the degree sequence check.
 *   2 means they failed the 'invariant' check.
 *   3 means they failed the canonical strings of ear-deleted subgraphs check.
 */
int GraphData::compare(GraphData* gd)
{
	/* Check vertices / edges */
	if ( this->n != gd->n || this->e != gd->e )
	{
		return 0;
	}
	/* passed vertices / edges */

	/* check degrees */

	if ( this->deg_seq == 0 )
	{
		this->computeDegSeq();
	}

	if ( gd->deg_seq == 0 )
	{
		gd->computeDegSeq();
	}

	for ( int i = 0; i < this->n; i++ )
	{
		if ( this->deg_seq[i] != gd->deg_seq[i] )
		{
			return 1;
		}
	}

	/* passed degree sequence check */

	/* check invariant */
	if ( this->invariant < 0 )
	{
		this->computeInvariant();
	}

	if ( gd->invariant < 0 )
	{
		gd->computeInvariant();
	}

	if ( this->invariant != gd->invariant )
	{
		return 2;
	}

	/* passed the invariant check */

	/* check canon_strings */
	if ( this->canon_strings == 0 )
	{
		this->computeCanonStrings();
	}

	if ( gd->canon_strings == 0 )
	{
		gd->computeCanonStrings();
	}

	for ( int i = 0; i < this->e; i++ )
	{
		if ( strcmp(this->canon_strings[i], gd->canon_strings[i]) != 0 )
		{
			return 3;
		}
	}

	/* passed ALL checks! */

	return -1;
}

/**
 * computeDegSeq()
 *
 * Compute the sorted degree sequence from the graph.
 */
void GraphData::computeDegSeq()
{
	/* compute the sorted degree sequence by selection sort (n < 15, probably) */

	this->deg_seq = (int*) malloc(this->n * sizeof(int));

	int cur_index = 0;
	int cur_value = this->n;

	while ( cur_index < this->n )
	{
		int select_max = -1;
		int select_count = 0;
		for ( int i = 0; i < this->n; i++ )
		{
			if ( this->g->d[i] < cur_value && this->g->d[i] > select_max )
			{
				select_max = this->g->d[i];
				select_count = 1;
			}
			else if ( this->g->d[i] == select_max )
			{
				select_count++;
			}
		}

		for ( int i = 0; i < select_count; i++ )
		{
			this->deg_seq[cur_index + i] = select_max;
		}

		cur_index += select_count;
		cur_value = select_max;
	}
}

/**
 * computeInvariant()
 *
 * Compute the invariant based on the edge-cards.
 */
void GraphData::computeInvariant()
{
	/* TODO: implement the invariant function */
	this->invariant = 0.0;

	int** pair_used = (int**) malloc(this->g->nv * sizeof(int*));

	for ( int i = 0; i < this->g->nv; i++ )
	{
		pair_used[i] = (int*) malloc(this->g->nv * sizeof(int));
		bzero(pair_used[i], this->g->nv * sizeof(int));
	}

	for ( int i = 0; i < this->g->nv; i++ )
	{
		int degi = this->g->d[i];

		for ( int j = 0; j < degi; j++ )
		{
			int vertj = this->g->e[this->g->v[i] + j];
			int degj = this->g->d[vertj];

			if ( degj < degi )
			{
				/* increase this count! */
				(pair_used[degj][degi])++;
			}
			else
			{
				/* increase this count! */
				(pair_used[degi][degj])++;
			}
		}
	}

	for ( int i = 0; i < this->g->nv; i++ )
	{
		for ( int j = i + 1; j < this->g->nv; j++ )
		{
			if ( pair_used[i][j] > 0 )
			{
				(this->invariant) += pow(j, i * pair_used[i][j]);
				(this->invariant) += pow(this->n * i + j, ((double) pair_used[i][j]) * 0.125);
			}
		}

		free(pair_used[i]);
		pair_used[i] = 0;
	}

	free(pair_used);
	pair_used = 0;
}

/**
 * computeCanonStrings()
 *
 * Compute the sorted list of canonical strings of edge-deleted subgraphs.
 */
void GraphData::computeCanonStrings()
{
	/* TODO: Implement! */
	this->canon_strings = (char**) malloc(this->e * sizeof(char*));

	/* for all undirected edges (use {u,v} with u < v) */
	int edge_num = 0;
	for ( int i = 0; i < this->n; i++ )
	{
		for ( int j = 0; j < this->g->d[i]; j++ )
		{
			int edge = this->g->v[i] + j;
			int rev_edge = 0;
			int vertj = this->g->e[edge];

			if ( vertj > i )
			{
				for ( int k = 0; k < this->g->d[vertj]; k++ )
				{
					rev_edge = this->g->v[vertj] + k;

					if ( this->g->e[rev_edge] == i )
					{
						/* this is the correct value of rev_edge */
						break;
					}
				}
				/* compute canonstring here! */

				this->g->e[edge] = -1;
				this->g->e[rev_edge] = -1;

				char* canon_str = getCanonString(this->g);

				this->g->e[edge] = vertj;
				this->g->e[rev_edge] = i;

				/* find the place for canon_str */
				if ( edge_num == 0 || strcmp(this->canon_strings[edge_num - 1], canon_str) <= 0 )
				{
					/* insert at the end */
					this->canon_strings[edge_num] = canon_str;
				}
				else
				{
					/* use binary search to find insertion point! */
					int cur_left = 0;
					int cur_right = edge_num;
					int cur_pos = edge_num / 2;

					while ( cur_left < cur_right )
					{
						int compare = strcmp(this->canon_strings[cur_pos], canon_str);

						if ( compare > 0 )
						{
							/* move to the left */
							cur_right = cur_pos - 1;
							cur_pos = (cur_right + cur_left) / 2;
						}
						else if ( compare < 0 )
						{
							/* move to the right */
							cur_left = cur_pos + 1;
							cur_pos = (cur_right + cur_left) / 2;
						}
						else
						{
							/* EQAUL! as good as any */
							/* collapse the interval */
							cur_left = cur_pos;
							cur_right = cur_pos;
						}
					}

					/* at this spot, shift over and insert */
					for ( int k = edge_num - 1; k >= cur_pos; k-- )
					{
						this->canon_strings[k + 1] = this->canon_strings[k];
					}
					this->canon_strings[cur_pos] = canon_str;
				}

				edge_num++;
			}
		}
	}

	if ( edge_num < this->e )
	{
		printf("--Only checked %d edges, when should have checked %d...\n", edge_num, this->e);
	}

	/* Note: the strings are sorted by insertion sort, as we go along */
}

/**
 * Constructor -- initialize!
 */
EarNode::EarNode() :
	SearchNode(0)
{
	this->graph = 0;
	this->ear_length = 0;
	this->ear = 0;
	this->num_ears = 0;
	this->ear_list = 0;
	this->solution_data = 0;
	this->extendable = false;
	this->curChild = -1;
	this->reconstructible = false;
	this->numMatchings = 2;
	this->orbitList = 0;
	this->canonicalLabels = 0;
	this->adj_matrix_data = 0;
	this->prune_called = false;

	this->any_adj_zero = true;
	this->any_adj_two = false;
	this->dom_vert = false;
	this->copy_of_H = false;
	this->barriers = 0;
	this->num_barriers = 0;

	this->violatingPairs = new TreeSet();

	this->num_child_data = 0;
	this->size_child_data = 1000;
	this->child_data = (GraphData**) malloc(this->size_child_data * sizeof(GraphData*));
}

/**
 * Constructor -- initialize!
 */
EarNode::EarNode(LONG_T label) :
	SearchNode(label)
{
	this->graph = 0;
	this->ear_length = 0;
	this->ear = 0;
	this->n = 0;
	this->prune_called = false;
	this->num_ears = 0;
	this->ear_list = 0;
	this->solution_data = 0;
	this->extendable = false;
	this->reconstructible = false;
	this->curChild = -1;
	this->numMatchings = 2;
	this->orbitList = 0;
	this->adj_matrix_data = 0;
	this->barriers = 0;
	this->num_barriers = 0;
	this->any_adj_zero = true;
	this->any_adj_two = false;
	this->dom_vert = false;
	this->copy_of_H = false;
	this->canonicalLabels = 0;
	this->violatingPairs = new TreeSet();
	this->num_child_data = 0;
	this->size_child_data = 100;
	this->child_data = (GraphData**) malloc(this->size_child_data * sizeof(GraphData*));
}

/**
 * Destructor
 */
EarNode::~EarNode()
{
	if ( this->graph != 0 )
	{
		SG_FREE((*(this->graph)));
		free(this->graph);
		this->graph = 0;
	}

	if ( this->solution_data != 0 )
	{
		free(this->solution_data);
		this->solution_data = 0;
	}

	if ( this->ear_list != 0 )
	{
		for ( int i = 0; i < this->num_ears; i++ )
		{
			if ( this->ear == this->ear_list[i] )
			{
				this->ear = 0;
			}

			free(this->ear_list[i]);
			this->ear_list[i] = 0;
		}

		free(this->ear_list);
	}

	if ( this->ear != 0 )
	{
		free(this->ear);
		this->ear = 0;
	}

	if ( this->orbitList != 0 )
	{
		for ( int i = 0; i < this->numPairOrbits; i++ )
		{
			free(this->orbitList[i]);
		}
		free(this->orbitList);
		this->orbitList = 0;
	}

	if ( this->canonicalLabels != 0 )
	{
		free(this->canonicalLabels);
	}

	if ( this->child_data != 0 )
	{
		for ( int i = 0; i < this->num_child_data; i++ )
		{
			delete this->child_data[i];
			this->child_data[i] = 0;
		}

		free(this->child_data);
		this->child_data = 0;
		this->num_child_data = 0;
		this->size_child_data = 0;
	}

	if ( this->adj_matrix_data != 0 )
	{
		for ( int i = 0; i < this->n; i++ )
		{
			if ( this->adj_matrix_data[i] != 0 )
			{
				free(this->adj_matrix_data[i]);
				this->adj_matrix_data[i] = 0;
			}
		}

		free(this->adj_matrix_data);
		this->adj_matrix_data = 0;
	}

	if ( this->violatingPairs != 0 )
	{
		delete this->violatingPairs;
		this->violatingPairs = 0;
	}

	if ( this->barriers != 0 )
	{
		for ( int i = 0; i < this->num_barriers; i++ )
		{
			delete this->barriers[i];
			this->barriers[i] = 0;
		}

		free(this->barriers);
		this->barriers = 0;
		this->num_barriers = 0;
	}

	this->ear_length = 0;
}

/**
 * addGraphData
 *
 * Create a new GraphData object for 'g' and add it to the list.
 *
 * The specific data in GraphData will only be filled when necessary,
 * 	that is, when the earliest checks have failed.
 */
void EarNode::addGraphData(sparsegraph* g)
{
	if ( this->num_child_data == this->size_child_data )
	{
		(this->size_child_data) += 100;
		this->child_data = (GraphData**) realloc(this->child_data, this->size_child_data * sizeof(GraphData*));
	}

	this->child_data[this->num_child_data] = new GraphData(g);

	(this->num_child_data)++;
}

/**
 * initAdjMatrix -- fill the adj_matrix_data arrays
 */
void EarNode::initAdjMatrix()
{
	if ( this->graph == 0 )
	{
		return;
	}

	this->n = this->graph->nv;

	this->adj_matrix_data = (int**) malloc(this->graph->nv * sizeof(int*));
	bzero(this->adj_matrix_data, this->graph->nv * sizeof(int*));

	for ( int i = 0; i < this->graph->nv; i++ )
	{
		this->adj_matrix_data[i] = (int*) malloc((this->graph->nv + 1) * sizeof(int));
		bzero(this->adj_matrix_data[i], (this->graph->nv + 1) * sizeof(int));
	}

	for ( int i = 0; i < this->graph->nv; i++ )
	{
		/* for all adjacencies, add -1's */
		for ( int j = 0; j < this->graph->d[i]; j++ )
		{
			int vertj = this->graph->e[this->graph->v[i] + j];

			//			if ( vertj < i )
			//			{
			/* fill in the adjacency */

			this->adj_matrix_data[i][vertj] = -1;
			this->adj_matrix_data[vertj][i] = -1;

			//			}
		}
	}
}

/**
 * augment
 *
 * Given a sparsegraph, create a new one with a new ear
 * 	of the given length between the two vertices.
 */
sparsegraph* EarSearchManager::augment(sparsegraph* graph, int length, int v1, int v2)
{

	return 0;
}

/**
 * Constructor
 *
 * @param algorithm the algorithm for checking pruning.
 * @param checker the algorithm for checking solutions.
 * @param deleter the algorithm for checking canonical deletions.
 */
EarSearchManager::EarSearchManager(int max_n, PruningAlgorithm* algorithm, SolutionChecker* checker,
		EarDeletionAlgorithm* deleter) :
	SearchManager()
{
	this->max_n = max_n;
	this->pruner = algorithm;
	this->checker = checker;
	this->deleter = deleter;

	this->require_all_siblings = checker->require_all_siblings;

	if ( this->stack.empty() == false )
	{
		this->stack.clear();
	}

	if ( this->root != 0 )
	{
		delete this->root;
	}

	this->root = new EarNode(0);
	((EarNode*) this->root)->max_verts = this->max_n;

	/* set to a two cycle so that three cycle is the start */
	this->root->curChild = 2;

	//	this->stack.push_back(this->root);
}

/**
 * Destructor
 */
EarSearchManager::~EarSearchManager()
{

}

/**
 * importArguments -- take the command line arguments
 * 	and convert them into options.
 *
 * This includes the following options:
 * 	mode: generate or run
 *  stages: -s [count] [s0] [s1] ... [sk]
 *  killtime: -k [killtime]
 *  maxdepth: -m [maxdepth]
 *
 *  It is set as so that one can strip
 *   custom arguments, if necessary.
 */
void EarSearchManager::importArguments(int argc, char** argv)
{
	super::importArguments(argc, argv);
}

/**
 * pushNext -- deepen the search to the next child
 * 	of the current node.
 *
 * @return the label for the new node. -1 if none.
 */
LONG_T EarSearchManager::pushNext()
{
	if ( this->stack.size() == 0 )
	{
		return this->pushTo(this->root->curChild + 1);
	}

	EarNode* parent = (EarNode*) this->stack.back();

	sparsegraph* g = parent->graph;

	int length, orbit;

	if ( parent->curChild == -1 )
	{
		length = 0;
		orbit = -1;
	}
	else
	{
		getInfoFromLabel(parent->curChild, length, orbit);
	}

	int orb1, orb2;
	do
	{
		orbit++;
		/* TODO: test orbit for being allowed */

		if ( orbit >= parent->numPairOrbits )
		{
			length++;
			orbit = 0;

			/* Bound by parent->max_verts (derrickstolee 02/04/2011) */
			if ( length + g->nv > parent->max_verts )
			{
				/* time to stop branching here */
				return -1;
			}
		}

		indexToPair(g->nv, parent->orbitList[orbit][0], orb1, orb2);

		/* if length == 0 and these are adjacent, we must continue */
		/* Also, if this is a "violating pair" then we must continue */
	} while ( (length == 0 && areAdjacent(g, orb1, orb2)) || parent->violatingPairs->contains(
			parent->orbitList[orbit][0]) == 1 );

	if ( (length == 0 && areAdjacent(g, orb1, orb2)) || parent->violatingPairs->contains(parent->orbitList[orbit][0])
			== 1 )
	{
		/* this is a problem! */
		/* we can't add a multiedge */
		/* OR this pair is a violating pair! */
		return -1;
	}

	LONG_T label = parent->curChild = getLabel(length, orbit);

	const int old_n = g->nv;
	g->nv = old_n + length;
	g->nde = g->nde + 2 * (length + 1);

	const int v1 = orb1;
	const int v2 = orb2;

	const int d1 = g->d[v1];
	const int d2 = g->d[v2];

	g->d[v1] = d1 + 1;
	g->d[v2] = d2 + 1;

	if ( length > 0 )
	{
		g->e[g->v[v1] + d1] = old_n;
		g->e[g->v[v2] + d2] = old_n + length - 1;

		for ( int i = 0; i < length; i++ )
		{
			g->d[old_n + i] = 2;

			if ( i == 0 )
			{
				g->e[g->v[old_n + i]] = v1;
			}
			else
			{
				g->e[g->v[old_n + i]] = old_n + i - 1;
			}

			if ( i == length - 1 )
			{
				g->e[g->v[old_n + i] + 1] = v2;
			}
			else
			{
				g->e[g->v[old_n + i] + 1] = old_n + i + 1;
			}
		}
	}
	else
	{
		/* just adding an edge */
		g->e[g->v[v1] + d1] = v2;
		g->e[g->v[v2] + d2] = v1;
	}

	/* MAKE A NEW NODE! */
	EarNode* node = new EarNode(label);
	node->max_verts = parent->max_verts;
	node->graph = g;

	/* EAR INFORMATION */
	node->ear_length = length;
	node->ear = (char*) malloc(length + 4);
	node->ear[0] = v1;

	for ( int i = 0; i < length; i++ )
	{
		node->ear[i + 1] = old_n + i;
	}

	node->ear[length + 1] = v2;
	node->ear[length + 2] = -1;
	node->ear[length + 3] = -1;

	/* EAR LIST */
	if ( this->stack.size() == 1 )
	{
		/* correction for when starting with a cycle */
		node->num_ears = 3;
		node->ear_list = (char**) malloc(3 * sizeof(char*));

		node->ear_list[0] = (char*) malloc(old_n + 1);
		node->ear_list[1] = (char*) malloc(old_n + 1);
		node->ear_list[2] = node->ear;

		int new_length = old_n;

		int last_ear_0 = v2;
		int last_ear_1 = v1;

		if ( v2 < v1 )
		{
			last_ear_0 = v2 + new_length;
		}
		else
		{
			last_ear_1 = v1 + new_length;
		}

		int eindex = 0;
		for ( int i = v1; i <= last_ear_0; i++ )
		{
			node->ear_list[0][eindex] = i % old_n;
			eindex++;
		}
		node->ear_list[0][eindex] = -1;

		eindex = 0;
		for ( int i = v2; i <= last_ear_1; i++ )
		{
			node->ear_list[1][eindex] = i % old_n;
			eindex++;
		}
		node->ear_list[1][eindex] = -1;
	}
	else
	{
		/* we have a "nice" ear decomposition already */
		node->num_ears = parent->num_ears + 1; /* add one for the new ear */

		if ( d1 == 2 )
		{
			/* a new ear on this endpoint! */
			node->num_ears = node->num_ears + 1;
		}

		if ( d2 == 2 )
		{
			/* a new ear on this endpoint! */
			node->num_ears = node->num_ears + 1;
		}

		node->ear_list = (char**) malloc(node->num_ears * sizeof(char*));

		int ear_index = 0;
		int v1splits = 0;
		int v2splits = 0;

		for ( int i = 0; i < parent->num_ears; i++ )
		{
			node->ear_list[ear_index] = (char*) malloc(g->nv);
			int inner_index = 0;

			for ( int j = 0; parent->ear_list[i][j] >= 0; j++ )
			{
				node->ear_list[ear_index][inner_index] = parent->ear_list[i][j];

				inner_index++;

				if ( inner_index > 1 && parent->ear_list[i][j] == v1 && j > 0 && parent->ear_list[i][j + 1] >= 0 )
				{
					/* if an internal vertex is also an endpoint of the new ear */
					/* split the ear here! */
					node->ear_list[ear_index][inner_index] = -1;
					ear_index++;

					v1splits++;

					node->ear_list[ear_index] = (char*) malloc(g->nv);
					inner_index = 0;
					node->ear_list[ear_index][inner_index] = v1;
					inner_index++;
				}
				else if ( inner_index > 1 && j > 0 && parent->ear_list[i][j + 1] >= 0 && parent->ear_list[i][j] == v2 )
				{
					/* if an internal vertex is also an endpoint of the new ear */
					/* split the ear here! */
					node->ear_list[ear_index][inner_index] = -1;
					ear_index++;

					v2splits++;

					node->ear_list[ear_index] = (char*) malloc(g->nv);
					inner_index = 0;

					node->ear_list[ear_index][inner_index] = v2;
					inner_index++;
				}
			}

			node->ear_list[ear_index][inner_index] = -1;
			ear_index++;
		}

		node->ear_list[ear_index] = node->ear;
		ear_index++;

		if ( ear_index != node->num_ears )
		{
			printf("--[pushNext] ear_index = %d != %d = node->num_ears w/ %d v1(%d) splits and %d v2(%d) splits.\n",
					ear_index, node->num_ears, v1splits, v1, v2splits, v2);

			exit(1);
		}
	}

	/* CANONICAL INFORMATION */
	node->canonicalLabels = (int*) malloc(2 * g->nv * sizeof(int));
	bzero(node->canonicalLabels, 2 * g->nv * sizeof(int));

	node->orbitList = (int**) malloc(g->nv * g->nv * sizeof(int*));
	bzero(node->orbitList, g->nv * g->nv * sizeof(int*));

	node->numPairOrbits = getCanonicalLabelsAndPairOrbits(g, node->canonicalLabels, node->orbitList);

	this->stack.push_back(node);

	return node->label;
}

/**
 * pushTo -- deepen the search to the specified child
 * 	of the current node.
 *
 * @param child the specified label for the new node
 * @return the label for the new node. -1 if none, or failed.
 */
LONG_T EarSearchManager::pushTo(LONG_T child)
{
	int param_length, param_orbit;

	getInfoFromLabel(child, param_length, param_orbit);

	if ( this->stack.size() == 0 )
	{
		/* at the BASE level */
		EarNode* ear_root = (EarNode*) this->root;

		this->root->curChild = child;

		if ( root->curChild > this->max_n )
		{
			return -1;
		}
		else if ( this->root->curChild < 3 )
		{
			this->root->curChild = 3;
		}

		int cycle_l = (int) this->root->curChild;
		//		printf("--[pushNext] \t\t MAKING A CYCLE OF LENGTH %d.\n", cycle_l);

		EarNode* node = new EarNode(cycle_l);
		node->max_verts = ear_root->max_verts;

		/* Need to make a cycle! */
		node->graph = (sparsegraph*) malloc(sizeof(sparsegraph));

		/* g for short */
		sparsegraph* g = node->graph;
		SG_INIT((*g));
		g->nv = cycle_l;
		g->nde = 2 * cycle_l;

		/* THESE ARE NOT BEING FREED? */
		g->v = (size_t*) malloc(this->max_n * sizeof(size_t));
		g->d = (int*) malloc(this->max_n * sizeof(int));
		g->e = (int*) malloc(this->max_n * this->max_n * sizeof(int));

		bzero(g->v, this->max_n * sizeof(int));
		bzero(g->d, this->max_n * sizeof(int));
		bzero(g->e, this->max_n * this->max_n * sizeof(int));

		for ( int i = 0; i < ear_root->max_verts; i++ )
		{
			g->v[i] = i * this->max_n;

			for ( int j = 0; j < this->max_n; j++ )
			{
				g->e[g->v[i] + j] = -1;
			}

			if ( i < cycle_l )
			{
				g->d[i] = 2;
				g->e[g->v[i]] = (i + 1) % (cycle_l);
				g->e[g->v[i] + 1] = (i + cycle_l - 1) % cycle_l;
			}
			else
			{
				g->d[i] = 0;
			}
		}

		/* the pair orbits are easy */
		node->canonicalLabels = (int*) malloc(2 * g->nv * sizeof(int));

		node->orbitList = (int**) malloc(g->nv * g->nv * sizeof(int*));
		bzero(node->orbitList, g->nv * g->nv * sizeof(int*));

		node->numPairOrbits = getCanonicalLabelsAndPairOrbits(g, node->canonicalLabels, node->orbitList);

		/* initialize the ear data... */
		/* n^2 / 2 ears at most */
		/* currently, the ears always end at 0, because they will always split there */
		node->num_ears = 1;
		node->ear_list = 0;
		node->ear_length = cycle_l - 1; /* length does not include endpoints */
		node->ear = 0; /* but the list does */

		this->stack.push_back(node);
		//		this->writeJob(stdout);

		return cycle_l;
	}

	if ( this->require_all_siblings )
	{
		/* get all previous siblings on the way to the current child */
		LONG_T label = this->pushNext();
		while (label >= 0 && label != child )
		{
			/* go through the actions for this previous sibling */
			if ( this->prune() == 0 )
			{
				this->isSolution();
			}

			this->pop();

			label = this->pushNext();
		}

		return label;
	}

	EarNode* parent = (EarNode*) this->stack.back();

	sparsegraph* g = parent->graph;

	int length, orbit;

	parent->curChild = child;

	int orb1, orb2;

	orbit = param_orbit;
	length = param_length;
	indexToPair(g->nv, parent->orbitList[orbit][0], orb1, orb2);

	if ( (length == 0 && areAdjacent(g, orb1, orb2)) || parent->violatingPairs->contains(parent->orbitList[orbit][0])
			== 1 )
	{
		/* this is a problem! */
		/* we can't add a multiedge */
		/* OR this pair is a violating pair! */
		return -1;
	}

	LONG_T label = parent->curChild = getLabel(length, orbit);

	const int old_n = g->nv;
	g->nv = old_n + length;
	g->nde = g->nde + 2 * (length + 1);

	const int v1 = orb1;
	const int v2 = orb2;

	const int d1 = g->d[v1];
	const int d2 = g->d[v2];

	g->d[v1] = d1 + 1;
	g->d[v2] = d2 + 1;

	if ( length > 0 )
	{
		g->e[g->v[v1] + d1] = old_n;
		g->e[g->v[v2] + d2] = old_n + length - 1;

		for ( int i = 0; i < length; i++ )
		{
			g->d[old_n + i] = 2;

			if ( i == 0 )
			{
				g->e[g->v[old_n + i]] = v1;
			}
			else
			{
				g->e[g->v[old_n + i]] = old_n + i - 1;
			}

			if ( i == length - 1 )
			{
				g->e[g->v[old_n + i] + 1] = v2;
			}
			else
			{
				g->e[g->v[old_n + i] + 1] = old_n + i + 1;
			}
		}
	}
	else
	{
		/* just adding an edge */
		g->e[g->v[v1] + d1] = v2;
		g->e[g->v[v2] + d2] = v1;
	}

	/* MAKE A NEW NODE! */
	EarNode* node = new EarNode(label);
	node->max_verts = parent->max_verts;
	node->graph = g;

	/* EAR INFORMATION */
	node->ear_length = length;
	node->ear = (char*) malloc(length + 4);
	node->ear[0] = v1;

	for ( int i = 0; i < length; i++ )
	{
		node->ear[i + 1] = old_n + i;
	}

	node->ear[length + 1] = v2;
	node->ear[length + 2] = -1;
	node->ear[length + 3] = -1;

	/* EAR LIST */
	if ( this->stack.size() == 1 )
	{
		/* correction for when starting with a cycle */
		node->num_ears = 3;
		node->ear_list = (char**) malloc(3 * sizeof(char*));

		node->ear_list[0] = (char*) malloc(old_n + 1);
		node->ear_list[1] = (char*) malloc(old_n + 1);
		node->ear_list[2] = node->ear;

		int new_length = old_n;

		int last_ear_0 = v2;
		int last_ear_1 = v1;

		if ( v2 < v1 )
		{
			last_ear_0 = v2 + new_length;
		}
		else
		{
			last_ear_1 = v1 + new_length;
		}

		int eindex = 0;
		for ( int i = v1; i <= last_ear_0; i++ )
		{
			node->ear_list[0][eindex] = i % old_n;
			eindex++;
		}
		node->ear_list[0][eindex] = -1;

		eindex = 0;
		for ( int i = v2; i <= last_ear_1; i++ )
		{
			node->ear_list[1][eindex] = i % old_n;
			eindex++;
		}
		node->ear_list[1][eindex] = -1;
	}
	else
	{
		/* we have a "nice" ear decomposition already */
		node->num_ears = parent->num_ears + 1; /* add one for the new ear */

		if ( d1 == 2 )
		{
			/* a new ear on this endpoint! */
			node->num_ears = node->num_ears + 1;
		}

		if ( d2 == 2 )
		{
			/* a new ear on this endpoint! */
			node->num_ears = node->num_ears + 1;
		}

		node->ear_list = (char**) malloc(node->num_ears * sizeof(char*));

		int ear_index = 0;
		int v1splits = 0;
		int v2splits = 0;

		for ( int i = 0; i < parent->num_ears; i++ )
		{
			node->ear_list[ear_index] = (char*) malloc(g->nv);
			int inner_index = 0;

			for ( int j = 0; parent->ear_list[i][j] >= 0; j++ )
			{
				node->ear_list[ear_index][inner_index] = parent->ear_list[i][j];

				inner_index++;

				if ( inner_index > 1 && parent->ear_list[i][j] == v1 && j > 0 && parent->ear_list[i][j + 1] >= 0 )
				{
					/* if an internal vertex is also an endpoint of the new ear */
					/* split the ear here! */
					node->ear_list[ear_index][inner_index] = -1;
					ear_index++;

					v1splits++;

					node->ear_list[ear_index] = (char*) malloc(g->nv);
					inner_index = 0;
					node->ear_list[ear_index][inner_index] = v1;
					inner_index++;
				}
				else if ( inner_index > 1 && j > 0 && parent->ear_list[i][j + 1] >= 0 && parent->ear_list[i][j] == v2 )
				{
					/* if an internal vertex is also an endpoint of the new ear */
					/* split the ear here! */
					node->ear_list[ear_index][inner_index] = -1;
					ear_index++;

					v2splits++;

					node->ear_list[ear_index] = (char*) malloc(g->nv);
					inner_index = 0;

					node->ear_list[ear_index][inner_index] = v2;
					inner_index++;
				}
			}

			node->ear_list[ear_index][inner_index] = -1;
			ear_index++;
		}

		node->ear_list[ear_index] = node->ear;
		ear_index++;

		if ( ear_index != node->num_ears )
		{
			printf("--[pushNext] ear_index = %d != %d = node->num_ears w/ %d v1(%d) splits and %d v2(%d) splits.\n",
					ear_index, node->num_ears, v1splits, v1, v2splits, v2);

			exit(1);
		}
	}

	/* CANONICAL INFORMATION */
	node->canonicalLabels = (int*) malloc(2 * g->nv * sizeof(int));
	bzero(node->canonicalLabels, 2 * g->nv * sizeof(int));

	node->orbitList = (int**) malloc(g->nv * g->nv * sizeof(int*));
	bzero(node->orbitList, g->nv * g->nv * sizeof(int*));

	node->numPairOrbits = getCanonicalLabelsAndPairOrbits(g, node->canonicalLabels, node->orbitList);

	this->stack.push_back(node);

	return node->label;
}

/**
 * pop -- remove the current node and move up the tree.
 *
 * @return the label of the node after the pop.
 * 		This return value is used for validation purposes
 * 		to check proper implementation of push*() and pop().
 */
LONG_T EarSearchManager::pop()
{
	if ( this->stack.empty() )
	{
		return -1;
	}

	EarNode* node = (EarNode*) this->stack.back();
	this->stack.pop_back();

	if ( node->graph != 0 )
	{
		if ( this->stack.size() == 0 )
		{
			/* free the graph at the end */
			SG_FREE((*(node->graph)));
			free(node->graph);
			node->graph = 0;
		}
		else if ( this->stack.size() > 0 )
		{
			sparsegraph* g = node->graph;

			/* don't let it get freed */
			node->graph = 0;

			/* but, it must be modified to remove other vertices! */
			int length = node->ear_length;

			g->nv = g->nv - length;
			g->nde = g->nde - 2 * (length + 1);
			g->d[(int)node->ear[0]] = g->d[(int)node->ear[0]] - 1;
			g->d[(int)node->ear[length + 1]] = g->d[(int)node->ear[length + 1]] - 1;

			for ( int i = 0; i < length; i++ )
			{
				g->d[(int)node->ear[i + 1]] = 0;
			}
		}
	}

	LONG_T label = node->label;

	delete node;

	return label;
}

/**
 * prune -- Perform a check to see if this node should be pruned.
 *
 * @return 0 if no prune should occur, 1 if prune should occur.
 */
int EarSearchManager::prune()
{

	if ( this->stack.size() > 1 )
	{
		EarNode* top = (EarNode*) this->stack.back();
		this->stack.pop_back();

		EarNode* parent = (EarNode*) this->stack.back();

		this->stack.push_back(top);

		/* prune is faster than deleter to check */
		int value = this->pruner->checkPrune(parent, top);

		if ( value == 1 )
		{
			return 1;
		}

		/* next, check canonical deletion */
		int parent_data_size = parent->num_child_data;
		int canon_ear = this->deleter->getCanonical(parent, top);

		bool added_child_data = false;
		if ( parent->num_child_data == parent_data_size + 1 )
		{
			added_child_data = true;
		}

		if ( canon_ear < 0 )
		{
			/* deleter failed! */
			printf("--[EarSearchManager] The deleter gave a negative orbit...\n");

			if ( added_child_data )
			{
				/* remove the excess graph data! */
				delete parent->child_data[parent->num_child_data - 1];
				parent->num_child_data = parent->num_child_data - 1;
			}
			return 1;
		}

		/*  check orbits of ears! */
		int orb1 = top->ear_list[canon_ear][0];

		int orb2 = 0;
		int i = 1;
		for ( i = 1; top->ear_list[canon_ear][i] >= 0; i++ )
		{
			orb2 = top->ear_list[canon_ear][i];
		}

		if ( i - 2 != top->ear_length )
		{
			/* wrong ear length! */
			if ( added_child_data )
			{
				/* remove the excess graph data! */
				delete parent->child_data[parent->num_child_data - 1];
				parent->num_child_data = parent->num_child_data - 1;
			}
			return 1;
		}

		/* get the pair index for the ear that was added */
		int epair = pairToIndex(top->graph->nv, top->ear[0], top->ear[top->ear_length + 1]);

		/* get the pair index for the orbit which was selected as canonical */
		int opair = pairToIndex(top->graph->nv, orb1, orb2);

		if ( epair != opair )
		{
			/* check for orbit equality! */
			for ( int j = 0; j < top->numPairOrbits; j++ )
			{
				int num_contains = 0;

				for ( int k = 0; top->orbitList[j][k] >= 0; k++ )
				{
					if ( top->orbitList[j][k] == epair )
					{
						num_contains++;
					}
					else if ( top->orbitList[j][k] == opair )
					{
						num_contains++;
					}
				}

				if ( num_contains == 1 )
				{
					/* not canonical deletion as endpoint pairs are in different sets! */
					if ( added_child_data )
					{
						/* remove the excess graph data! */
						delete parent->child_data[parent->num_child_data - 1];
						parent->num_child_data = parent->num_child_data - 1;
					}
					return 1;
				}
				else if ( num_contains == 2 )
				{
					/* is canonical! */
					//					printf("-- put (%d,%d) in orbit with (%d,%d)...\n",
					//							top->ear[0], top->ear[top->ear_length + 1], orb1,
					//							orb2);
					//printf("\t\t%s", sgtos6(top->graph) );
					return 0;
				} /* if found only one else if found both */
			} /* for all orbits */

		} /* if epair != opair */

		return 0;
	}
	else if ( this->stack.size() == 1 )
	{
		/* no deletion to check, just prune */
		EarNode* top = (EarNode*) this->stack.back();
		return this->pruner->checkPrune((EarNode*) this->root, top);
	}

	return 0;
}

/**
 * isSolution -- Perform a check to see if a solution exists
 * 		at this point.
 *
 * @return 0 if no solution is found, 1 if a solution is found.
 */
int EarSearchManager::isSolution()
{
	EarNode* top = (EarNode*) this->stack.back();

	this->stack.pop_back();
	EarNode* parent = (EarNode*) this->stack.back();
	this->stack.push_back(top);

	char* solution_data = this->checker->isSolution(parent, top, this->stack.size());

	if ( solution_data == 0 )
	{
		return 0;
	}
	else
	{
		/* this is a workaround so solutions are printed */
		/* but the search doesn't stop */
		super::writeSolutionJob(stdout);
		printf("%s", solution_data);
		return 0;
	}
	//
	//	if ( top->solution_data != 0 )
	//	{
	//		free(top->solution_data);
	//		top->solution_data = 0;
	//	}
	//
	//	top->solution_data = (char*) malloc(strlen(solution_data) + 1);
	//	strcpy(top->solution_data, solution_data);
	//
	//	return 1;
}

/**
 * writeSolution -- create a buffer that contains a
 * 	description of the solution.
 *
 * Solution strings start with S.
 *
 * @return a string of data allocated with malloc().
 * 	It will be deleted using free().
 */
char* EarSearchManager::writeSolution()
{
	EarNode* top = (EarNode*) this->stack.back();

	if ( top->solution_data != 0 )
	{
		char* buffer = (char*) malloc(strlen(top->solution_data) + 1);
		strcpy(buffer, top->solution_data);
		return buffer;
	}

	return 0;
}

/**
 * writeStatistics -- create a buffer that contains a
 * 	description of the solution.
 *
 * Statistics take the following format in each line:
 *
 * T [TYPE] [ID] [VALUE]
 *
 * @return a string of data allocated with malloc().
 * 	It will be deleted using free().
 */
char* EarSearchManager::writeStatistics()
{
	char* buffer = super::writeStatistics();
	int buf_len = strlen(buffer);

	int buf_pos = buf_len;

	buf_len += 2000 + 500 * this->maxDepth + 500 * this->checker->getMaxDepth();
	buffer = (char*) realloc(buffer, buf_len);

	int total_solutions = 0;

	for ( int i = 0; i < this->checker->getMaxDepth(); i++ )
	{
		int numSolsi = this->checker->getNumSolutionsAt(i);
		total_solutions += numSolsi;

		sprintf(&(buffer[buf_pos]), "T SUM NUM_SOLS_AT_%d %d\n", i, numSolsi);
		buf_pos += strlen(&(buffer[buf_pos]));
	}

	this->checker->clearSolutions();
	sprintf(&(buffer[buf_pos]), "T SUM NUM_SOLS %d\n", total_solutions);
	buf_pos += strlen(&(buffer[buf_pos]));

	char* checkBuff = this->checker->writeStatisticsData();
	int check_len = strlen(checkBuff);
	if ( buf_pos + check_len + 1 > buf_len )
	{
		buf_len += check_len + 10;
		buffer = (char*) realloc(buffer, buf_len);
	}
	sprintf(&(buffer[buf_pos]), "%s", checkBuff);
	free(checkBuff);

	return buffer;
}
