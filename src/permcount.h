/*
 * permcount.h
 *
 *  Created on: Oct 16, 2010
 *      Author: derrickstolee
 */

#ifndef PERMCOUNT_H_
#define PERMCOUNT_H_

#include "nausparse.h"


int getConjC(int p);


/**
 * Return the number of perfect matchings in the sparsegraph g.
 *
 * It will consider vertices i with g->v[i] < 0 as "deleted" and not available (allows for recursion).
 *
 * If the number of perfect matchings exceeds max_p, returns that number, which may not be the full count.
 *
 */
int countPM(sparsegraph* g, int max_p);


/**
 * Return the number of perfect matchings in the sparsegraph g.
 *
 * It will consider vertices i with g->v[i] < 0 as "deleted" and not available (allows for recursion).
 *
 * If the number of perfect matchings exceeds max_p, returns that number, which may not be the full count.
 *
 */
int countPM(sparsegraph* g, int max_p, int* labels);



/**
 * Return true iff the sparsegraph g has a perfect matching.
 *
 * It will consider vertices i with g->v[i] < 0 as "deleted" and not available (allows for recursion). *
 */
bool hasPM(sparsegraph* g);


/**
 * hasPM -- find a perfect matching and return the labels of the edges in a list.
 *
 */
bool hasPM(sparsegraph* g, int* labels);

#endif /* PERMCOUNT_H_ */
