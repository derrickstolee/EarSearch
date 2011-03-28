/*
 * pairs.h
 *
 *  Created on: Oct 28, 2010
 *      Author: derrickstolee
 */

#ifndef PAIRS_H_
#define PAIRS_H_

/*************************************/
/********TRANSLATION METHODS**********/
/*************************************/

/**
 * pairToIndex
 *
 * @param n The total number of vertices.
 * @param v1
 * @param v2
 * @return the index for edge {v1, v2}
 */
int pairToIndex(int n, int v1, int v2);

/**
 * indexToPair
 *
 * @param n The total number of vertices.
 * @param index
 * @param v1
 * @param v2
 * @return the index for edge {v1, v2}
 */
void indexToPair(int n, int index, int& v1, int& v2);

#endif /* PAIRS_H_ */
