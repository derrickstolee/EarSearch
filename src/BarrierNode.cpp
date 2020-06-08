/*
 * BarrierNode.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: derrickstolee
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nausparse.h"
#include "Set.hpp"
#include "TreeSet.hpp"
#include "BarrierNode.hpp"

/**
 * Constructor with a set of vertices and the components.
 *
 * This is used to construct barriers for C_2k.
 */
BarrierNode::BarrierNode(Set* verts, int n, int* components)
{
	this->elt_array = 0;
	this->elements = new TreeSet();
	this->n = n;
	this->next = 0;
	this->components = (int*) malloc(n * sizeof(int));
	bcopy(components, this->components, n * sizeof(int));

	for ( verts->resetIterator(); verts->hasNext(); )
	{
		int elt = verts->next();
		this->elements->add(elt);
		this->components[elt] = -1;
	}

	this->size = this->elements->size();
}

/**
 * Constructor from a BarrierNode, and a set of NEW vertices from an ear.
 *
 * @param base the BarrierNode containing a barrier in H.
 * @param ear_list the vertices of an ear added (in order).
 * @param num_ears the number of ears
 * @param ear_length the number of internal vertices in the ear.
 * @param ear_elts the vertices to add to this barrier.
 * @param the number of vertices in ear_elts.
 */
BarrierNode::BarrierNode(BarrierNode* base, int** ear_list, int num_ears, int* ear_lengths, int usable_ear,
		int* ear_elts, int num_ear_elts)
{
	this->elt_array = 0;
	this->n = base->n;

	for ( int i = 0; i < num_ears; i++ )
	{
		/* ear_lengths is the number of internal vertices */
		this->n = this->n + ear_lengths[i];
	}

	this->components = (int*) malloc(this->n * sizeof(int));
	bcopy(base->components, this->components, base->n * sizeof(int));

	this->elements = new TreeSet();

	for ( base->elements->resetIterator(); base->elements->hasNext(); )
	{
		int elt = base->elements->next();
		this->elements->add(elt);
		this->components[elt] = -1;
	}

	for ( int i = 0; i < num_ear_elts; i++ )
	{
		this->elements->add(ear_elts[i]);
	}

	bool used_first_new_component = false;
	int num_components = base->size;
	int mode = 0;

	for ( int i = 0; i < num_ears; i++ )
	{
		if ( i != usable_ear )
		{
			/* this component matches ear_list[i][0] */
			for ( int j = 1; j <= ear_lengths[i]; j++ )
			{
				this->components[ear_list[i][j]] = this->components[ear_list[i][0]];
			}
		}
	}

	int* ear = 0;
	if ( usable_ear >= 0 )
	{
		ear = ear_list[usable_ear];

		if ( this->elements->contains(ear[0]) == 1 )
		{
			mode = 1;
			/* this barrier contains the 0 endpoint of the ear */
			int end_component = this->components[ear[ear_lengths[usable_ear] + 1]];
			int last_i = ear_lengths[usable_ear];

			for ( int i = ear_lengths[usable_ear]; i > 0 && this->elements->contains(ear[i]) == 0; i-- )
			{
				/* for things not in the ear, use the component for the ear[ear_length+1] */
				this->components[ear[i]] = end_component;
				last_i = i;
			}

			for ( int i = 1; i < last_i; i++ )
			{
				if ( this->elements->contains(ear[i]) )
				{
					this->components[ear[i]] = -1;

					if ( used_first_new_component )
					{
						num_components++;
					}
				}
				else
				{
					used_first_new_component = true;
					this->components[ear[i]] = num_components;
				}
			}

			if ( used_first_new_component )
			{
				num_components++;
			}
		}
		else if ( this->elements->contains(ear[ear_lengths[usable_ear] + 1]) != 0 )
		{
			mode = 2;
			/* this barrier contains the 0 endpoint of the ear */
			int end_component = this->components[ear[0]];
			int last_i = 1;

			for ( int i = 1; i <= ear_lengths[usable_ear] && this->elements->contains(ear[i]) == 0; i++ )
			{
				/* for things not in the ear, use the component for the ear[0] */
				this->components[ear[i]] = end_component;
				last_i = i;
			}

			for ( int i = ear_lengths[usable_ear]; i > last_i; i-- )
			{
				if ( this->elements->contains(ear[i]) )
				{
					this->components[ear[i]] = -1;

					if ( used_first_new_component )
					{
						num_components++;
					}
				}
				else
				{
					used_first_new_component = true;
					this->components[ear[i]] = num_components;
				}
			}

			if ( used_first_new_component )
			{
				num_components++;
			}
		}
		else
		{
			mode = 3;
			/* neither end point... */
			/* this barrier contains the 0 end point of the ear */
			int end_component = this->components[ear[0]];

			/* it's one */
			num_components++;

			if ( end_component != this->components[ear[ear_lengths[usable_ear] + 1]] )
			{
				printf("-- making a barrier with different endpoint components.\n");
			}

			int first_i = 1;

			for ( int i = 0; i <= ear_lengths[usable_ear] && this->elements->contains(ear[i]) == 0; i++ )
			{
				/* for things not in the ear, use the component for the ear[0] */
				this->components[ear[i]] = end_component;
				first_i = i;
			}

			int last_i = 1;
			for ( int i = ear_lengths[usable_ear] + 1; i > 0 && this->elements->contains(ear[i]) == 0; i-- )
			{
				/* for things not in the ear, use the component for the ear[ear_length+1] */
				this->components[ear[i]] = end_component;
				last_i = i;
			}

			/* first_i + 1 is in the set */
			/* last_i - 1 is in the set */
			for ( int i = first_i + 1; i < last_i; i++ )
			{
				if ( this->elements->contains(ear[i]) )
				{
					this->components[ear[i]] = -1;

					if ( used_first_new_component )
					{
						used_first_new_component = false;
						num_components++;
					}
				}
				else
				{
					used_first_new_component = true;
					this->components[ear[i]] = num_components;
				}
			}

			if ( used_first_new_component )
			{
				num_components++;
			}
		}
	} /* end if usable_ear >= 0 */

	this->size = this->elements->size();

	if ( num_components != this->size )
	{
		printf("-- num_components (%d) != (%d) this->size. Usable ear: %d\t Mode: %d\n", num_components, this->size,
				usable_ear, mode);

		printf("-- EAR: ");
		for ( int i = 0;i  <= ear_lengths[usable_ear] + 1; i++ )
		{
			printf("\t(%d,%d)", ear[i],this->components[ear[i]]);
		}
		printf("\n");
	}
}

/**
 * Destructor
 */
BarrierNode::~BarrierNode()
{
	this->n = 0;
	free(this->components);
	this->components = 0;

	delete this->elements;
	this->elements = 0;

	this->size = 0;
	this->next = 0;

	if ( this->elt_array != 0 )
	{
		free(this->elt_array);
		this->elt_array = 0;
	}
}

/**
 * getElements
 *
 * @return the set of elements of the barrier.
 */
Set* BarrierNode::getElements()
{
	return this->elements;
}

/**
 * getComponents
 */
int* BarrierNode::getComponents()
{
	return this->components;
}

/**
 * splitsEar
 *
 * Check if this barrier splits the given ear across two components.
 *
 * @param ear The ear to add to H
 * @param ear_length the number of vertices in the ear
 * @return True iff the endpoints of ear are in different components of B
 */
bool BarrierNode::splitsEar(int* ear, int ear_length)
{
	if ( this->components[ear[0]] != this->components[ear[ear_length + 1]] )
	{
		return true;
	}

	return false;
}

/**
 * conflicts
 *
 * Check if this barrier conflicts with another barrier:
 *
 * 1) if they intersect.
 * 2) if b is in multiple components of H-B.
 *
 * @param b the other barrier.
 */
bool BarrierNode::conflicts(BarrierNode* b)
{
	Set* elts = b->getElements();
	elts->resetIterator();

	int elt = elts->next();

	if ( this->elements->contains(elt) != 0 )
	{
		return true;
	}
	int min_component = this->components[elt];

	while ( elts->hasNext() )
	{
		elt = elts->next();

		if ( this->elements->contains(elt) != 0 )
		{
			return true;
		}
		if ( this->components[elt] != min_component )
		{
			return true;
		}
	}

	/* check other direction */
	elts = this->getElements();
	elts->resetIterator();

	elt = elts->next();

	if ( b->elements->contains(elt) != 0 )
	{
		return true;
	}
	min_component = b->components[elt];

	while ( elts->hasNext() )
	{
		elt = elts->next();

		if ( b->elements->contains(elt) != 0 )
		{
			return true;
		}
		if ( b->components[elt] != min_component )
		{
			return true;
		}
	}

	return false;
}

/**
 * constructBarriers
 *
 * Given a list of barriers of a graph,
 * 	and a list of new ears (1 or 2 new ears),
 *  compute the barriers for the new graph.
 * This is an in-line action,
 * 	so it doesn't need the graph.
 */
BarrierNode** constructBarriers(BarrierNode** b_list, int num_barriers, int** ear_list, int* ear_length, int num_ears,
		int& num_new_barriers)
{
	int size_new_b_list = num_barriers;
	int tnum_new_barriers = 0;
	BarrierNode** new_b_list = (BarrierNode**) malloc(size_new_b_list * sizeof(BarrierNode*));

	for ( int i = 0; i < num_barriers; i++ )
	{
		BarrierNode* b = b_list[i];

		if ( b->getSize() == 0 )
		{
			/*
			 * add new barriers for the new ear vertices
			 */
			for ( int i = 0; i < num_ears; i++ )
			{
				/* for each ear */

				for ( int direction = -1; direction <= 1; direction += 2 )
				{
					/* for each direction */
					int endpoint = 0;

					if ( direction == -1 )
					{
						endpoint = ear_length[i] + 1;
					}

					/* need to add for all subsets of even-distance points from end point */
					int max_sub_size = ear_length[i] / 2;
					int sub_indicator = 0;
					int* ear = ear_list[i];
					int max_indicator = 1 << max_sub_size;
					int* ear_elts = (int*) malloc(max_sub_size * sizeof(int));

					while ( sub_indicator < max_indicator )
					{
						/* add the barrier for this indicator */
						int num_ear_elts = 0;

						for ( int j = 1; j <= max_sub_size; j++ )
						{
							if ( (sub_indicator & (1 << (j - 1))) != 0 )
							{
								/* add vert at 2*i distance from the endpoint in the barrier */
								ear_elts[num_ear_elts] = ear[endpoint + direction * 2 * j];
								num_ear_elts++;
							}
						}

						if ( num_ear_elts > 0 )
						{
							/* add a new barrier */
							if ( tnum_new_barriers >= size_new_b_list )
							{
								size_new_b_list += num_barriers;
								new_b_list
										= (BarrierNode**) realloc(new_b_list, size_new_b_list * sizeof(BarrierNode*));
							}

							/* Add this barrier */
							new_b_list[tnum_new_barriers] = new BarrierNode(b, ear_list, num_ears, ear_length, i,
									ear_elts, num_ear_elts);
							tnum_new_barriers++;
						}

						sub_indicator++;
					} /* end while */

					free(ear_elts);
					ear_elts = 0;
				}
			}
		}
		else
		{
			/* test if this barrier should be removed */
			bool remove_barrier = false;
			int usable_ear = -1;
			int endpoint = -1;
			int direction = 0;
			for ( int j = 0; j < num_ears && !remove_barrier; j++ )
			{
				int* ear = ear_list[j];
				/* if both end points are in this barrier, throw it away! */
				if ( b->getElements()->contains(ear[0]) != 0 && b->getElements()->contains(ear[ear_length[j] + 1]) != 0 )
				{
					remove_barrier = true;
				}
				else if ( b->getElements()->contains(ear[0]) != 0 )
				{
					usable_ear = j;
					endpoint = 0;
					direction = 1;
				}
				else if ( b->getElements()->contains(ear[ear_length[j] + 1]) != 0 )
				{
					usable_ear = j;
					endpoint = ear_length[j] + 1;
					direction = -1;
				}
				else if ( b->getComponents()[ear[0]] != b->getComponents()[ear[ear_length[j] + 1]] )
				{
					/* this ear spans multiple components, and neither are in the set */
					remove_barrier = true;
				}
			}

			if ( !remove_barrier )
			{
				if ( tnum_new_barriers >= size_new_b_list )
				{
					size_new_b_list += num_barriers;
					new_b_list = (BarrierNode**) realloc(new_b_list, size_new_b_list * sizeof(BarrierNode*));
				}

				/* Make a new barrier */
				new_b_list[tnum_new_barriers] = new BarrierNode(b, ear_list, num_ears, ear_length, usable_ear, 0, 0);
				tnum_new_barriers++;

				if ( usable_ear >= 0 )
				{
					/* need to add for all subsets of even-distance points from end point */
					int max_sub_size = ear_length[usable_ear] / 2;
					int sub_indicator = 0;
					int* ear = ear_list[usable_ear];
					int max_indicator = 1 << max_sub_size;
					int* ear_elts = (int*) malloc(max_sub_size * sizeof(int));

					while ( sub_indicator < max_indicator )
					{
						/* add the barrier for this indicator */
						int num_ear_elts = 0;

						for ( int i = 1; i <= max_sub_size; i++ )
						{
							if ( (sub_indicator & (1 << (i - 1))) != 0 )
							{
								/* add vert at 2*i distance from the endpoint in the barrier */
								ear_elts[num_ear_elts] = ear[endpoint + direction * 2 * i];
								num_ear_elts++;
							}
						}

						if ( num_ear_elts > 0 )
						{
							/* add a new barrier */
							if ( tnum_new_barriers >= size_new_b_list )
							{
								size_new_b_list += num_barriers;
								new_b_list
										= (BarrierNode**) realloc(new_b_list, size_new_b_list * sizeof(BarrierNode*));
							}

							/* Add this barrier */
							new_b_list[tnum_new_barriers] = new BarrierNode(b, ear_list, num_ears, ear_length,
									usable_ear, ear_elts, num_ear_elts);
							tnum_new_barriers++;
						}

						sub_indicator++;
					} /* end while */

					free(ear_elts);
					ear_elts = 0;
				}
			}
		} /* if b->size > 0 */
	}

	num_new_barriers = tnum_new_barriers;
	return new_b_list;
}

/**
 * getArray
 *
 * get an array of the elements
 */
int* BarrierNode::getArray()
{
	if ( this->elt_array == 0 )
	{
		this->elt_array = (int*) malloc((this->size + 1) * sizeof(int));
		this->elements->resetIterator();

		int i = 0;
		while ( this->elements->hasNext() )
		{
			this->elt_array[i] = this->elements->next();
			i++;
		}

		this->elt_array[i] = -1;
	}
	return this->elt_array;
}

/**
 * getSize
 *
 * Get the size of the barrier.
 */
int BarrierNode::getSize()
{
	return this->size;
}
