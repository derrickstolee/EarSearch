/*
 * ReconstructionDeleter.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include "ReconstructionDeleter.hpp"
#include "connectivity.hpp"
#include "canonstring.hpp"

/**
 * Constructor
 *
 * @param n the maximum number of vertices allowed.
 */
ReconstructionDeleter::ReconstructionDeleter(int n)
{

}

/**
 * Destructor
 */
ReconstructionDeleter::~ReconstructionDeleter()
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
int ReconstructionDeleter::getCanonical(EarNode* parent, EarNode* child)
{
	sparsegraph* graph = child->graph;
	int* labels = child->canonicalLabels;
	int num_ears = child->num_ears;
	char** ear_list = child->ear_list;

	/* Begin by attempting to prove that 'graph' is already reconstructible */
	bool reconstructible = false;

	if ( graph == 0 || graph->nv < n )
	{
		/* let's not check the lower orders */
		reconstructible = true;
	}
	else
	{
		/* graph is regular */
		int reg_degree = graph->d[0];
		for ( int i = 1; i < graph->nv; i++ )
		{
			if ( graph->d[i] != reg_degree )
			{
				reg_degree = -1;
				break;
			}
		}

		/**
		 * If the graph is regular,
		 * 	it is reconstructible.
		 */
		if ( reg_degree > 0 )
		{
			reconstructible = true;
		}
	}

	if ( !reconstructible )
	{
		/* min-degree is adjacent to min-used-degree */
		/* get min-degree of graph */

		int** pair_used = (int**) malloc(graph->nv * sizeof(int*));

		for ( int i = 0; i < graph->nv; i++ )
		{
			pair_used[i] = (int*) malloc(graph->nv * sizeof(int));
			bzero(pair_used[i], graph->nv * sizeof(int));
		}

		/* ALSO CHECK: some vertex has all incident ears non-trivial */
		bool some_vert_all_trivial = false;
		bool some_vert_all_nontrivial = false;

		for ( int i = 0; i < graph->nv; i++ )
		{
			int degi = graph->d[i];
			bool vert_i_all_trivial = true;

			for ( int j = 0; j < degi; j++ )
			{
				int vertj = graph->e[graph->v[i] + j];
				int degj = graph->d[vertj];

				if ( degj == 2 )
				{
					vert_i_all_trivial = false;
				}

				/* which ear is this? */
				char ear_ij[3];
				ear_ij[0] = i;
				ear_ij[1] = vertj;
				ear_ij[2] = -1;

				if ( !is2connected(graph, i, vertj, (char*) ear_ij) )
				{
					/* don't count this non-2-connected graph! */
					continue;
				}

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

			if ( vert_i_all_trivial )
			{
				some_vert_all_trivial = true;
			}
			else
			{
				some_vert_all_nontrivial = true;
			}
		}

		/** ATTEMPT DELETING AN EDGE **/
		/* compute the degree pair {d1,d2} of edges which
		 * 	delete to 2-connected graphs
		 *  with minimum multiplicity */
		int min_used_deg1 = -1;
		int min_used_deg2 = -1;
		int min_used_count = graph->nv * graph->nv;

		for ( int i = 0; i < graph->nv; i++ )
		{
			for ( int j = 0; j < graph->nv; j++ )
			{
				if ( pair_used[i][j] > 0 && pair_used[i][j] < min_used_count )
				{
					min_used_count = pair_used[i][j];
					min_used_deg1 = i;
					min_used_deg2 = j;
				}
			}

			free(pair_used[i]);
			pair_used[i] = 0;
		}
		free(pair_used);
		pair_used = 0;

		/**
		 * If there is a branch vertex
		 * 	incident to only non-trivial ears,
		 * 	then it is reconstructible.
		 * (Proposition 4.2.6. (2))
		 */
		if ( some_vert_all_nontrivial == true )
		{
			reconstructible = true;
		}

		if ( !reconstructible )
		{
			/* ear-length large enough */
			for ( int i = 0; i < num_ears; i++ )
			{
				int lengthi = 0;

				/* ears are -1 terminated */
				while ( ear_list[i][lengthi] >= 0 )
				{
					lengthi++;
				}

				/**
				 * If there exists an ear with at least two
				 * 	internal vertices, then G is reconstructible.
				 * (Proposition 4.2.6 (1))
				 */
				if ( lengthi > 2 )
				{
					/* ear too long! */
					reconstructible = true;

					break;
				}
			}

			/* no vertex has all incident ears trivial */
			if ( !reconstructible )
			{
				/* Not necessarily reconstructible. */
				/* We need to find a canonical deletion based only on the edge-deck */
				char* best_string = 0;
				int best_ear_index = 0;
				int best_ear_length = 2;

				if ( min_used_deg1 > 0 )
				{
					/* there actually is an edge which deletes to a 2-connected graph */

					bool min_used_deg_diff = (min_used_deg1 != min_used_deg2);

					/* consider all edges between such vertices */
					/* FOR ALL VERTS OF MIN DEGREE */
					for ( int ear_index = 0; ear_index < num_ears; ear_index++ )
					{
						/* the ear better be length 1  */
						if ( ear_list[ear_index][2] >= 0 )
						{
							/* too long! */
							continue;
						}

						int verti = ear_list[ear_index][0];
						int vertj = ear_list[ear_index][1];

						if ( graph->d[verti] > graph->d[vertj] )
						{
							int t = verti;
							verti = vertj;
							vertj = t;
						}

						if ( graph->d[verti] != min_used_deg1 )
						{
							continue;
						}

						if ( graph->d[vertj] != min_used_deg2 )
						{
							continue;
						}

						/* Is it 2-connected without this ear? */
						/* which ear is this? */
						char ear_ij[3];
						ear_ij[0] = verti;
						ear_ij[1] = vertj;
						ear_ij[2] = -1;

						if ( !is2connected(graph, verti, vertj, (char*) ear_ij) )
						{
							/* this isn't one of the counted ears */
							continue;
						}

						/* Get edge-deleted subgraph */

						int edge = -1;
						int rev_edge = -1;

						for ( int k = 0; k < min_used_deg1; k++ )
						{
							if ( graph->e[graph->v[verti] + k] == vertj )
							{
								edge = graph->v[verti] + k;
								break;
							}
						}
						for ( int k = 0; k < min_used_deg2; k++ )
						{
							if ( graph->e[graph->v[vertj] + k] == verti )
							{
								rev_edge = graph->v[vertj] + k;
								break;
							}
						}

						graph->e[edge] = -1;
						graph->e[rev_edge] = -1;

						/* Compute its canonical string */
						char* canon_string_e = getCanonString(graph);

						graph->e[edge] = vertj;
						graph->e[rev_edge] = verti;

						/* Is this the best string so far? */
						if ( best_string == 0 )
						{
							best_string = canon_string_e;
							best_ear_index = ear_index;
						}
						else if ( strcmp(best_string, canon_string_e) > 0 )
						{
							char* temp_string = best_string;
							best_string = canon_string_e;
							best_ear_index = ear_index;

							/* cleanup */
							free(temp_string);
							temp_string = canon_string_e = 0;
						}
						else
						{
							free(canon_string_e);
							canon_string_e = 0;
						}

					}

					/**
					 * Add 'graph' to the parent's list!
					 * Only want to do it in this case, not the reconstructible case.
					 */
					parent->addGraphData(graph);

					free(best_string);
					best_string = 0;

					return best_ear_index;
				}
				else
				{
					/* we need to delete an ear of length 2 */
					/* let's recompute the degree pairs for ears of length 2 */
					pair_used = (int**) malloc(graph->nv * sizeof(int*));

					for ( int i = 0; i < graph->nv; i++ )
					{
						pair_used[i] = (int*) malloc(graph->nv * sizeof(int));
						bzero(pair_used[i], graph->nv * sizeof(int));
					}

					for ( int i = 0; i < graph->nv; i++ )
					{
						int degi = graph->d[i];

						for ( int j = 0; j < degi; j++ )
						{
							int vertj = graph->e[graph->v[i] + j];
							int degj = graph->d[vertj];
							int vj = graph->v[vertj];

							if ( degj == 2 )
							{
								/* what is the other neighbor? */
								/* which ear is this? */
								char ear_ij[4];
								ear_ij[0] = i;
								ear_ij[1] = vertj;
								if ( graph->e[vj] == i )
								{
									ear_ij[2] = graph->e[vj + 1];
								}
								else
								{
									ear_ij[2] = graph->e[vj];
								}
								ear_ij[3] = -1;

								if ( !is2connected(graph, i, vertj, (char*) ear_ij) )
								{
									/* don't count this non-2-connected graph! */
									continue;
								}
								else if ( degj < degi )
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
					}

					/* compute the degree pair {d1,d2} of 2-ears which
					 * 	delete to 2-connected graphs
					 *  with minimum multiplicity */
					int min_used_deg1 = -1;
					int min_used_deg2 = -1;
					int min_used_count = graph->nv * graph->nv;

					for ( int i = 0; i < graph->nv; i++ )
					{
						for ( int j = 0; j < graph->nv; j++ )
						{
							if ( pair_used[i][j] > 0 && pair_used[i][j] < min_used_count )
							{
								min_used_count = pair_used[i][j];
								min_used_deg1 = i;
								min_used_deg2 = j;
							}
						}

						free(pair_used[i]);
						pair_used[i] = 0;
					}
					free(pair_used);
					pair_used = 0;

					bool min_used_deg_diff = (min_used_deg1 != min_used_deg2);

					/* consider all ears between such vertices */
					/* FOR ALL VERTS OF MIN DEGREE */
					for ( int ear_index = 0; ear_index < num_ears; ear_index++ )
					{
						/* the ear better be length 2  */
						if ( ear_list[ear_index][1] < 0 || ear_list[ear_index][2] >= 0 )
						{
							/* too short, or too long! */
							continue;
						}

						int verti = ear_list[ear_index][0];
						int vertb = ear_list[ear_index][1];
						int vertj = ear_list[ear_index][2];

						if ( graph->d[verti] > graph->d[vertj] )
						{
							int t = verti;
							verti = vertj;
							vertj = t;
						}

						if ( graph->d[verti] != min_used_deg1 )
						{
							continue;
						}

						if ( graph->d[vertj] != min_used_deg2 )
						{
							continue;
						}

						/* Is it 2-connected without this ear? */
						/* which ear is this? */
						char ear_ij[3];
						ear_ij[0] = verti;
						ear_ij[1] = vertb;
						ear_ij[2] = vertj;
						ear_ij[2] = -1;

						if ( !is2connected(graph, verti, vertj, (char*) ear_ij) )
						{
							/* this isn't one of the counted ears */
							continue;
						}

						/* Get edge-deleted subgraph, for both endpoints */

						int edge = -1;
						int rev_edge = -1;

						/* FIRST: Delete verti-vertb */
						for ( int k = 0; k < min_used_deg1; k++ )
						{
							if ( graph->e[graph->v[verti] + k] == vertb )
							{
								edge = graph->v[verti] + k;
								break;
							}
						}
						for ( int k = 0; k < 2; k++ )
						{
							if ( graph->e[graph->v[vertb] + k] == verti )
							{
								rev_edge = graph->v[vertb] + k;
								break;
							}
						}

						char* canon_string = 0;
						if ( min_used_count > 1 )
						{
							/* we need to distinguish by nauty */

							graph->e[edge] = -1;
							graph->e[rev_edge] = -1;
							graph->d[verti] = graph->d[verti] - 1;
							graph->d[vertb] = 1;

							/* Compute its canonical string */
							char* canon_string_e1 = getCanonString(graph);

							graph->e[edge] = vertb;
							graph->e[rev_edge] = verti;
							graph->d[verti] = graph->d[verti] + 1;
							graph->d[vertb] = 2;

							/* SECOND: Delete vertb-vertj */
							for ( int k = 0; k < min_used_deg2; k++ )
							{
								if ( graph->e[graph->v[vertj] + k] == vertb )
								{
									edge = graph->v[vertj] + k;
									break;
								}
							}
							for ( int k = 0; k < 2; k++ )
							{
								if ( graph->e[graph->v[vertb] + k] == vertj )
								{
									rev_edge = graph->v[vertb] + k;
									break;
								}
							}

							graph->e[edge] = -1;
							graph->e[rev_edge] = -1;
							graph->d[vertj] = graph->d[vertj] - 1;
							graph->d[vertb] = 1;

							char* canon_string_e2 = getCanonString(graph);

							graph->e[edge] = vertb;
							graph->e[rev_edge] = vertj;
							graph->d[vertj] = graph->d[vertj] + 1;
							graph->d[vertb] = 2;

							/* which is best? */
							int clen = strlen(canon_string_e1) + strlen(canon_string_e2) + 5;
							canon_string = (char*) malloc(clen);
							bzero(canon_string, clen);

							if ( strcmp(canon_string_e1, canon_string_e2) <= 0 )
							{
								strcpy(canon_string, canon_string_e1);
							}
							else
							{
								strcpy(canon_string, canon_string_e2);
							}

							free(canon_string_e1);
							canon_string_e1 = 0;
							free(canon_string_e2);
							canon_string_e2 = 0;
						}

						/* Is this the best string so far? */
						if ( best_string == 0 )
						{
							best_string = canon_string;
							best_ear_index = ear_index;
						}
						else if ( strcmp(best_string, canon_string) > 0 )
						{
							char* temp_string = best_string;
							best_string = canon_string;
							best_ear_index = ear_index;

							/* cleanup */
							free(temp_string);
							temp_string = canon_string = 0;
						}
						else
						{
							free(canon_string);
							canon_string = 0;
						}
					}

					printf("--deleting a 2-ear with card: %s", best_string);

					/**
					 * Add 'graph' to the parent's list!
					 * Only want to do it in this case, not the reconstructible case.
					 */
					parent->addGraphData(graph);

					if ( best_string != 0 )
					{
						free(best_string);
						best_string = 0;
					}

					return best_ear_index;
				}
			}
		}
	}

	/* HERE: WE ARE RECONSTRUCTIBLE */
	/* use the same algorithm as in the regular 2-connected case */

	int min_ear_length = graph->nv + 5;
	int min_ear_index = -1;
	int min_ear_label1 = graph->nv;
	int min_ear_label2 = graph->nv;

	for ( int i = 0; i < num_ears; i++ )
	{
		/* find ear length */
		int len = 0;
		for ( ; ear_list[i][len] >= 0; len++ )
		{

		}

		int ear_lab_1 = labels[ear_list[i][0]];
		int ear_lab_2 = labels[ear_list[i][len - 1]];

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
			if ( is2connected(graph, ear_list[i][0], ear_list[i][len - 1], ear_list[i]) )
			{
				if ( len < min_ear_length )
				{
					min_ear_length = len;
					min_ear_label1 = ear_lab_1;
					min_ear_label2 = ear_lab_2;
					min_ear_index = i;
				}
				else if ( ear_lab_1 < min_ear_label1 || ((ear_lab_1 == min_ear_label1) && (ear_lab_2 < min_ear_label2)) )
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
