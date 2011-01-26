/*
 * canonstring.hpp
 *
 *  Created on: Jan 24, 2011
 *      Author: derrickstolee
 */

#ifndef CANONSTRING_HPP_
#define CANONSTRING_HPP_

#include "nausparse.h"

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
 */
char* getCanonString(sparsegraph* g);

#endif /* CANONSTRING_HPP_ */
