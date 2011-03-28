/*
 * SaturationAlgorithm.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: derrickstolee
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gtools.h"
#include "pairs.h"

#include "PruningAlgorithm.hpp"
#include "SolutionChecker.hpp"
#include "SaturationAlgorithm.hpp"

char* modifiedGraphToS6(sparsegraph* g)
{
	/* need to make a better graph! */
	sparsegraph sg;
	SG_INIT(sg);

	sg.w = 0;
	sg.wlen = 0;

	sg.v = (int*) malloc(g->nv * sizeof(int));
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

	sg.vlen = vindex;
	sg.dlen = vindex;
	sg.elen = eindex;
	sg.nv = vindex;
	sg.nde = eindex;

	free(down_verts);
	free(rev_verts);

	char* buffer = sgtos6(&sg);

	SG_FREE(sg);

	return buffer;
}

/**
 * Constructor
 */
SaturationAlgorithm::SaturationAlgorithm(int mode, int output_mode, bool unique, int max_depth) :
	SolutionChecker(max_depth)
{
	this->mode = mode;
	this->output_mode = output_mode;
	this->unique = unique;
}

/**
 * Destructor
 */
SaturationAlgorithm::~SaturationAlgorithm()
{

}

/**
 * isSolution -- take a graph and check for a solution.
 * Return a string of data if it is a solution.
 *
 * @param parent the parent EarNode -- contains adjacent children already considered
 * @param child the child EarNode -- all data for the current node
 * @return a string of data, if it is a solution.  0 if not.
 */
char* SaturationAlgorithm::isSolution(EarNode* parent, EarNode* child, int depth)
{
	if ( child->prune_called == false )
	{
		int val = this->checkPrune(parent, child);
		if ( val == 1 )
		{
			return 0;
		}
	}

	if ( this->output_mode == SAT_OUTPUT_SOLUTIONS )
	{

		/* Check for copies of H in the child graph. */
		/* Use counts from the checkPrune algorithm. */
		if ( child == 0 || child->graph == 0 || child->graph->nv < 4 || child->any_adj_zero || child->any_adj_two
				|| child->copy_of_H || child->dom_vert )
		{
			return 0;
		}
	}
	else if ( this->output_mode == SAT_OUTPUT_ALL )
	{
		/* for this mode, cannot have copy of H */
		/* cannot have multiple copies of H when adding a non-edge */
		if ( child == 0 || child->graph == 0 || child->graph->nv < 4 || child->any_adj_two || child->copy_of_H )
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	/* get the graph! */
	char* str = modifiedGraphToS6(child->graph);
	char* buffer = (char*) malloc(strlen(str) + 1);
	strcpy(buffer, str);

	if ( depth >= 0 && depth < this->max_depth )
	{
		this->num_solutions_at[depth] = this->num_solutions_at[depth] + 1;
	}

	return buffer;
}

/**
 * checkPrune -- check if the given graph needs to be pruned.
 *
 * @return 1 iff the graph should be pruned.
 */
int SaturationAlgorithm::checkPrune(EarNode* parent, EarNode* child)
{
	if ( child == 0 || child->graph == 0 || child->graph->nv < 4 )
	{
		return 0;
	}

	child->prune_called = true;
	child->initAdjMatrix();
	child->any_adj_zero = false; /* here's hoping! it will be false if a zero appears */
	child->any_adj_two = false;
	child->copy_of_H = false;

	if ( (this->mode & GRAPH_K) == GRAPH_K )
	{
		/* COMPLETE MODE */
		/* don't care about dominating vertices! */
		child->dom_vert = false;
		for ( int i = 0; i < child->graph->nv; i++ )
		{
			if ( child->graph->d[i] == child->graph->nv - 1 )
			{
				child->dom_vert = true;

				if ( this->output_mode == SAT_OUTPUT_SOLUTIONS )
				{
					/* we can prune in this case, since all ear augmentations */
					/* from this point on will be edges, so no new vertices */
					/* and we don't care if there is a dominating vertex */
					return 1;
				}
				else
				{
					break;
				}
			}
		}

		/* k the number of vertices in the graph */
		int k = this->mode & 0xFF;

		if ( k == 4 )
		{
			/* if there is a K_4, then prune! */
			for ( int i = 3; i < child->graph->nv; i++ )
			{
				for ( int j = 2; j < i; j++ )
				{
					if ( child->adj_matrix_data[i][j] < 0 )
					{
						for ( int a = 1; a < j; a++ )
						{
							if ( child->adj_matrix_data[i][a] < 0 && child->adj_matrix_data[j][a] < 0 )
							{
								for ( int b = 0; b < a; b++ )
								{
									if ( child->adj_matrix_data[i][b] < 0 && child->adj_matrix_data[j][b] < 0
											&& child->adj_matrix_data[a][b] < 0 )
									{
										/* i,j,a,b form a K_4! */
										child->copy_of_H = true;
										return 1;
									}
								}
							}
						}
					}
				}
			}

			/* special implementation for k = 4 */

			/* in the unique case, we cannot use parent data */
			for ( int i = 1; i < child->graph->nv; i++ )
			{
				for ( int j = 0; j < i; j++ )
				{
					if ( child->adj_matrix_data[i][j] >= 0 ) /* i-j a non-edge */
					{
						/* we have a non-edge */
						/* now, we test for a UNIQUE K_4 */

						/* for all neighbors of vertex i */
						for ( int n1 = 0; n1 < child->graph->d[i]; n1++ )
						{
							int neigh1 = child->graph->e[child->graph->v[i] + n1];

							/* test that neigh1 in neighborhood of j */
							if ( child->adj_matrix_data[j][neigh1] < 0 ) /* j-neigh1 an edge */
							{
								/* for all OTHER neighbors of i */
								for ( int n2 = n1 + 1; n2 < child->graph->d[i]; n2++ )
								{
									int neigh2 = child->graph->e[child->graph->v[i] + n2];

									if ( child->adj_matrix_data[j][neigh2] < 0 ) /* j-neigh2 an edge */
									{
										/* OK, we found a pair of common neighbors! */
										/* ARE THEY ADJACENT TO EACH OTHER? */
										if ( child->adj_matrix_data[neigh2][neigh1] < 0 ) /* neigh1-neigh2 an edge */
										{
											/* this is a K_4! */
											child->adj_matrix_data[i][j] = child->adj_matrix_data[i][j] + 1;
											child->adj_matrix_data[j][i] = child->adj_matrix_data[j][i] + 1;

											if ( this->unique && child->adj_matrix_data[i][j] >= 2 ) /* too many K_4 copies for uniqueness! */
											{
												/* PRUNE THIS! */
												child->any_adj_two = true;
												return 1;
											}
										}
									}
								}
							}
						}
					}

					if ( child->adj_matrix_data[i][j] == 0 )
					{
						/* not YET a solution */
						child->any_adj_zero = true;
					}
				} /* for all j */
			} /* for all i  */
		} /* end K_4 */
		if ( k == 5 )
		{

			/* if there is a K_5, then prune! */
			for ( int i = 4; i < child->graph->nv; i++ )
			{
				for ( int j = 3; j < i; j++ )
				{
					if ( child->adj_matrix_data[i][j] < 0 )
					{
						for ( int a = 2; a < j; a++ )
						{
							if ( child->adj_matrix_data[i][a] < 0 && child->adj_matrix_data[j][a] < 0 )
							{
								for ( int b = 1; b < a; b++ )
								{
									if ( child->adj_matrix_data[i][b] < 0 && child->adj_matrix_data[j][b] < 0
											&& child->adj_matrix_data[a][b] < 0 )
									{
										for ( int c = 0; c < b; c++ )
										{

											if ( child->adj_matrix_data[i][c] < 0 && child->adj_matrix_data[j][c] < 0
													&& child->adj_matrix_data[a][c] < 0 && child->adj_matrix_data[b][c]
													< 0 )
											{
												/* i,j,a,b,c form a K_5! */
												child->copy_of_H = true;
												return 1;
											}
										}
									}
								}
							}
						}
					}
				}
			}

			/* special implementation for k = 5 */

			/* in the unique case, we cannot use parent data */
			for ( int i = 1; i < child->graph->nv; i++ )
			{
				for ( int j = 0; j < i; j++ )
				{
					if ( child->adj_matrix_data[i][j] >= 0 ) /* i-j a non-edge */
					{
						/* we have a non-edge */
						/* now, we test for a UNIQUE K_5 */

						/* for all neighbors of vertex i */
						for ( int n1 = 0; n1 < child->graph->d[i]; n1++ )
						{
							int neigh1 = child->graph->e[child->graph->v[i] + n1];

							/* test that neigh1 in neighborhood of j */
							if ( child->adj_matrix_data[j][neigh1] < 0 ) /* j-neigh1 an edge */
							{
								/* for all OTHER neighbors of i */
								for ( int n2 = n1 + 1; n2 < child->graph->d[i]; n2++ )
								{
									int neigh2 = child->graph->e[child->graph->v[i] + n2];

									if ( child->adj_matrix_data[j][neigh2] < 0
											&& child->adj_matrix_data[neigh2][neigh1] < 0 ) /* j-neigh2 an edge */
									{
										/* for all OTHER neighbors of i */
										for ( int n3 = n2 + 1; n3 < child->graph->d[i]; n3++ )
										{
											int neigh3 = child->graph->e[child->graph->v[i] + n3];

											if ( child->adj_matrix_data[j][neigh3] < 0
													&& child->adj_matrix_data[neigh3][neigh1] < 0
													&& child->adj_matrix_data[neigh3][neigh2] < 0 )
											{
												/* this is a K_5! */
												child->adj_matrix_data[i][j] = child->adj_matrix_data[i][j] + 1;
												child->adj_matrix_data[j][i] = child->adj_matrix_data[j][i] + 1;

												if ( this->unique && child->adj_matrix_data[i][j] >= 2 ) /* too many K_5 copies for uniqueness! */
												{
													/* PRUNE THIS! */
													child->any_adj_two = true;
													return 1;
												}
											} /* end if this completes a K_5 */
										} /* end for all third neighbors */
									} /* end if this forms a K_4 */
								} /* end for all second neighbors */
							} /* end if this is a common neighbor */
						} /* end for all neighbors */
					} /* end if a non-edge */

					if ( child->adj_matrix_data[i][j] == 0 )
					{
						/* not YET a solution */
						child->any_adj_zero = true;
					}
				} /* for all j */
			} /* for all i  */
		} /* end k == 5 */
		if ( k == 6 )
		{

			/* if there is a K_6, then prune! */
			for ( int i = 5; i < child->graph->nv; i++ )
			{
				for ( int j = 4; j < i; j++ )
				{
					if ( child->adj_matrix_data[i][j] < 0 )
					{
						for ( int a = 3; a < j; a++ )
						{
							if ( child->adj_matrix_data[i][a] < 0 && child->adj_matrix_data[j][a] < 0 )
							{
								for ( int b = 2; b < a; b++ )
								{
									if ( child->adj_matrix_data[i][b] < 0 && child->adj_matrix_data[j][b] < 0
											&& child->adj_matrix_data[a][b] < 0 )
									{
										for ( int c = 1; c < b; c++ )
										{

											if ( child->adj_matrix_data[i][c] < 0 && child->adj_matrix_data[j][c] < 0
													&& child->adj_matrix_data[a][c] < 0 && child->adj_matrix_data[b][c]
													< 0 )
											{
												for ( int d = 0; d < c; d++ )
												{
													if ( child->adj_matrix_data[i][d] < 0
															&& child->adj_matrix_data[j][d] < 0
															&& child->adj_matrix_data[a][d] < 0
															&& child->adj_matrix_data[b][d] < 0
															&& child->adj_matrix_data[c][d] < 0 )
													{
														/* i,j,a,b,c,d form a K_5! */
														child->copy_of_H = true;
														return 1;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			/* special implementation for k = 6 */

			/* in the unique case, we cannot use parent data */
			for ( int i = 1; i < child->graph->nv; i++ )
			{
				for ( int j = 0; j < i; j++ )
				{
					if ( child->adj_matrix_data[i][j] >= 0 ) /* i-j a non-edge */
					{
						/* we have a non-edge */
						/* now, we test for a UNIQUE K_6 */

						/* for all neighbors of vertex i */
						for ( int n1 = 0; n1 < child->graph->d[i]; n1++ )
						{
							int neigh1 = child->graph->e[child->graph->v[i] + n1];

							/* test that neigh1 in neighborhood of j */
							if ( child->adj_matrix_data[j][neigh1] < 0 ) /* j-neigh1 an edge */
							{
								/* for all OTHER neighbors of i */
								for ( int n2 = n1 + 1; n2 < child->graph->d[i]; n2++ )
								{
									int neigh2 = child->graph->e[child->graph->v[i] + n2];

									if ( child->adj_matrix_data[j][neigh2] < 0
											&& child->adj_matrix_data[neigh2][neigh1] < 0 ) /* j-neigh2 an edge */
									{
										/* for all OTHER neighbors of i */
										for ( int n3 = n2 + 1; n3 < child->graph->d[i]; n3++ )
										{
											int neigh3 = child->graph->e[child->graph->v[i] + n3];

											if ( child->adj_matrix_data[j][neigh3] < 0
													&& child->adj_matrix_data[neigh3][neigh1] < 0
													&& child->adj_matrix_data[neigh3][neigh2] < 0 )
											{
												/* for all OTHER neighbors of i */
												for ( int n4 = n3 + 1; n4 < child->graph->d[i]; n4++ )
												{
													int neigh4 = child->graph->e[child->graph->v[i] + n4];
													if ( child->adj_matrix_data[j][neigh4] < 0
															&& child->adj_matrix_data[neigh4][neigh1] < 0
															&& child->adj_matrix_data[neigh4][neigh2] < 0
															&& child->adj_matrix_data[neigh4][neigh3] < 0 )
													{
														/* this is a K_6! */
														child->adj_matrix_data[i][j] = child->adj_matrix_data[i][j] + 1;
														child->adj_matrix_data[j][i] = child->adj_matrix_data[j][i] + 1;

														if ( this->unique && child->adj_matrix_data[i][j] >= 2 ) /* too many K_6 copies for uniqueness! */
														{
															/* PRUNE THIS! */
															child->any_adj_two = true;
															return 1;
														}
													}
												}
											} /* end if this completes a K_5 */
										} /* end for all third neighbors */
									} /* end if this forms a K_4 */
								} /* end for all second neighbors */
							} /* end if this is a common neighbor */
						} /* end for all neighbors */
					} /* end if a non-edge */

					if ( child->adj_matrix_data[i][j] == 0 )
					{
						/* not YET a solution */
						child->any_adj_zero = true;
					}
				} /* for all j */
			} /* for all i  */
		} /* end k == 6 */
		else if ( k > 5 )
		{
			/* NOTE: THIS SECTION OF CODE IS FAULTY */
			/* check for LARGER K_k */
			/* if there is a K_k, then prune! */
			/* iterate over all k-sets, using local checks */
			for ( int i = 0; i < child->graph->nv; i++ )
			{
				if ( child->graph->d[i] >= k - 1 )
				{
					int deg = child->graph->d[i];
					int v_pos = child->graph->v[i];

					/* verts is an array of vertices */
					int* verts = (int*) malloc(k * sizeof(int));
					verts[0] = i;

					/* neighs is an array of neighbor positions */
					int* neighs = (int*) malloc(k * sizeof(int));

					for ( int j = 0; j < k; j++ )
					{
						neighs[j] = -1;
					}

					int num_in_verts = 1;

					do
					{
						if ( num_in_verts == k )
						{
							/* this is complete! */
							child->copy_of_H = true;
							//							printf("Complete graph on %d vertices: ", k);
							//							for ( int j = 0 ; j < k; j++ )
							//							{
							//								printf("(%d,%d) ", verts[j], neighs[j]);
							//							}
							//							printf("\n");

							free(verts);
							free(neighs);
							return 1;
						}

						bool found_common = false;
						for ( int j = neighs[num_in_verts] + 1; j < deg - (k - num_in_verts); j++ )
						{
							int neighbor = child->graph->e[v_pos + j];

							bool is_common = true;
							for ( int l = 1; l < num_in_verts; l++ )
							{
								if ( child->adj_matrix_data[verts[l]][neighbor] >= 0 )
								{
									/* a non-edge! */
									is_common = false;
									break;
								}
							}

							if ( is_common )
							{
								found_common = true;
								neighs[num_in_verts] = j;
								verts[num_in_verts] = neighbor;
								num_in_verts++;
								break;
							}
						}

						if ( !found_common )
						{
							/* reset */
							neighs[num_in_verts] = -1;
							num_in_verts--;
						}
					} while ( num_in_verts > 0 );

					free(verts);
					free(neighs);
				} /* if degree big enough, possibly */
			} /* for all i, looking for K_k */

			/* for all non-edges, make sure there is a complete k-2 clique in the common neighborhood */
			for ( int i = 0; i < child->graph->nv; i++ )
			{
				for ( int j = i + 1; j < child->graph->nv; j++ )
				{
					if ( child->adj_matrix_data[i][j] >= 0 )
					{
						/* non-edge! */
						int vert_i = i;
						int vert_j = j;
						int deg_i = child->graph->d[i];
						int deg_j = child->graph->d[j];

						if ( deg_j < deg_i )
						{
							int temp_v = vert_i;
							vert_i = vert_j;
							vert_j = temp_v;

							int temp_d = deg_i;
							deg_i = deg_j;
							deg_j = temp_d;
						}

						/* verts is an array of vertices */
						int* verts = (int*) malloc(k * sizeof(int));
						verts[0] = vert_i;
						verts[1] = vert_j;

						/* neighs is an array of neighbor positions */
						int* neighs = (int*) malloc(k * sizeof(int));

						for ( int l = 0; l < k; l++ )
						{
							neighs[l] = -1;
						}

						int num_in_verts = 2;

						do
						{
							if ( num_in_verts >= k )
							{
								/* this is complete! */
								/* increase the number! */
								child->adj_matrix_data[i][j] = child->adj_matrix_data[i][j] + 1;
								child->adj_matrix_data[j][i] = child->adj_matrix_data[j][i] + 1;

								if ( unique && child->adj_matrix_data[i][j] >= 2 )
								{
									child->any_adj_two = true;
									free(verts);
									free(neighs);
									return 1;
								}

								/* move this back to work on a new set */
								num_in_verts--;
							}

							bool found_common = false;
							for ( int a = neighs[num_in_verts] + 1; !found_common && a < deg_i - (k - num_in_verts); a++ )
							{
								int neighbor = child->graph->e[vert_i + a];

								if ( child->adj_matrix_data[vert_j][neighbor] >= 0 )
								{
									/* not common! */
									break;
								}

								bool is_common = true;
								for ( int l = 2; is_common && l < num_in_verts; l++ )
								{
									if ( child->adj_matrix_data[verts[l]][neighbor] >= 0 )
									{
										/* a non-edge! */
										is_common = false;
									}
								}

								if ( is_common )
								{
									found_common = true;
									neighs[num_in_verts] = j;
									verts[num_in_verts] = neighbor;
									num_in_verts++;
								}
							}

							if ( !found_common )
							{
								/* reset */
								neighs[num_in_verts] = -1;
								num_in_verts--;
							}
						} while ( num_in_verts > 1 );

						free(verts);
						free(neighs);

						if ( child->adj_matrix_data[i][j] == 0 )
						{
							child->any_adj_zero = true;
						}
					} /* if i-j non-edge */
				} /* end for all j */
			} /* end for all i */
		}
	} /* end  K */

	return 0;
}
