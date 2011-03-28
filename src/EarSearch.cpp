/*
 * EarSearch.cpp
 *
 *  Created on: Oct 15, 2010
 *      Author: derrickstolee
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EarSearchManager.hpp"
#include "PruningAlgorithm.hpp"
#include "SolutionChecker.hpp"
#include "EarDeletionAlgorithm.hpp"

#include "EnumeratePruner.hpp"
#include "EnumerateChecker.hpp"
#include "EnumerateDeleter.hpp"

#include "ReconstructionPruner.hpp"
#include "ReconstructionChecker.hpp"
#include "ReconstructionDeleter.hpp"

#include "MatchingPruner.hpp"
#include "MatchingChecker.hpp"
#include "MatchingDeleter.hpp"

#include "SaturationAlgorithm.hpp"

/**
 * The EarSearch program will run an instance of TreeSearch using
 * the EarSearchManager.  There are two modes:
 *
 * "--enumerate N e E" enumerates all 2-connected graphs with N vertices, and between
 *     e and E edges.
 *
 * "--reconstruction" searches over "sparse" two-connected graphs for
 * 	a counterexample to the edge-reconstruction conjecture.
 *
 * "--matchings p1 p2 n c" searches over 1-extendable graphs with at most
 * 		p perfect matchings and n vertices for a graph which grows
 * 		to an extremal elementary graph attaining a maximum "c_p" value.
 */
int main(int argc, char** argv)
{
	int mode = -1;
	int RECONST_MODE = 0;
	int PERFECT_MODE = 1;
	int ENUM_MODE = 2;
	int SATURATE_MODE = 3;

	int find_pmin = -1;
	int find_pmax = -1;
	int find_emin = -1;
	int find_emax = -1;
	int find_n = -1;
	int find_c = -1;

	PruningAlgorithm* pruner = 0;
	SolutionChecker* checker = 0;
	EarDeletionAlgorithm* deleter = 0;

	/* find mode and bounds */
	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp(argv[i], "--enumerate") == 0 )
		{
			mode = ENUM_MODE;

			if ( i > argc - 3 )
			{
				printf("Usage: --enumerate N e E \n");
				exit(1);
			}

			sscanf(argv[i + 1], "%d", &find_n);
			sscanf(argv[i + 2], "%d", &find_emin);
			sscanf(argv[i + 3], "%d", &find_emax);

			pruner = new EnumeratePruner(find_n, find_emin, find_emax);
			checker = new EnumerateChecker(find_n, find_emin, find_emax);
			deleter = new EnumerateDeleter();

			i += 3;
		}
		else if ( strcmp(argv[i], "--saturate") == 0 )
		{
			mode = SATURATE_MODE;

			if ( i > argc - 2 )
			{
				printf("Usage: --saturate k n\n");
				exit(1);
			}

			sscanf(argv[i + 1], "%d", &find_c);
			sscanf(argv[i + 2], "%d", &find_n);

			/* CURRENTLY: ONLY K_k UNIQUE SATURATION */
			SaturationAlgorithm* sat_alg = new SaturationAlgorithm((GRAPH_K | find_c),
					SAT_OUTPUT_SOLUTIONS,
					true, (find_n * find_n) / 2);

			checker = sat_alg;
			pruner = sat_alg;
			deleter = new EnumerateDeleter();

			i += 3;
		}
		else if ( strcmp(argv[i], "--satgen") == 0 )
		{
			mode = SATURATE_MODE;

			if ( i > argc - 2 )
			{
				printf("Usage: --satgen k n\n");
				exit(1);
			}

			sscanf(argv[i + 1], "%d", &find_c);
			sscanf(argv[i + 2], "%d", &find_n);

			/* CURRENTLY: ONLY K_k UNIQUE SATURATION */
			SaturationAlgorithm* sat_alg = new SaturationAlgorithm((GRAPH_K | find_c),
					SAT_OUTPUT_ALL,
					true, (find_n * find_n) / 2);

			checker = sat_alg;
			pruner = sat_alg;
			deleter = new EnumerateDeleter();

			i += 3;
		}
		else if ( strcmp(argv[i], "--reconstruction") == 0 )
		{
			mode = RECONST_MODE;

			if ( i > argc - 1 )
			{
				printf("Usage: --reconstruction n\n");
				exit(1);
			}

			sscanf(argv[i + 1], "%d", &find_n);

			pruner = new ReconstructionPruner(find_n);
			checker = new ReconstructionChecker(find_n);
			deleter = new ReconstructionDeleter(find_n);

			i += 1;
		}
		else if ( strcmp(argv[i], "--matchings") == 0 )
		{
			if ( i > argc - 5 )
			{
				printf("Usage: --matchings pmin pmax n c\n");
				exit(1);
			}

			sscanf(argv[i + 1], "%d", &find_pmin);
			sscanf(argv[i + 2], "%d", &find_pmax);
			sscanf(argv[i + 3], "%d", &find_n);
			sscanf(argv[i + 4], "%d", &find_c);

			mode = PERFECT_MODE;

			pruner = new MatchingPruner(find_pmin, find_pmax, find_n, find_c);
			checker = new MatchingChecker(find_pmin, find_pmax, find_n, find_c);
			deleter = new MatchingDeleter(find_pmin, find_pmax, find_n);

			i += 4;
		}
	}

	if ( mode < 0 )
	{
		printf("We require a mode! (--reconstruction n  or  --matchings p n or --saturate k n)\n");
		exit(1);
	}

	EarSearchManager* manager = new EarSearchManager(find_n, pruner, checker,
			deleter);

	manager->importArguments(argc, argv);

	while ( manager->readJob(stdin) >= 0 )
	{
		manager->doSearch();
	}

	delete manager;
	delete pruner;

	if ( mode != SATURATE_MODE )
	{
		delete checker;
	}

	delete deleter;

	return 0;
}
