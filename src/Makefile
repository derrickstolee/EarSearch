#
# Base Makefile for EarSearch project.
#
TESTS 	=


TARGETS = EarSearch.exe \
		deletiontester.exe


OBJECTS = 	BarrierAlgorithm.o		\
			BarrierNode.o			\
			BarrierSearch.o			\
			bipartite.o				\
			EarDeletionAlgorithm.o	\
			EarSearchManager.o		\
			pairs.o					\
			permcount.o 			\
			MatchingChecker.o		\
			MatchingDeleter.o		\
			MatchingPruner.o		\
			PruningAlgorithm.o		\
			ReconstructionChecker.o	\
			ReconstructionDeleter.o	\
			ReconstructionPruner.o	\
			SaturationAlgorithm.o	\
			Set.o			\
			SolutionChecker.o	\
			TreeSet.o		\
			EnumerateChecker.o      \
			EnumeratePruner.o       \
			EnumerateDeleter.o      \
			canonstring.o			\
			connectivity.o          \
			symmetry.o	

CC 	= gcc
CXX = g++

OPT = -O7 
DEBUG =  
# -g 
# -static 
WARNINGS = -Wall


CFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS)
CXXFLAGS 	= $(OPT) $(DEBUG) $(WARNINGS)
LFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS)


INCLUDES =  -I../treesearch \
			-I../nauty24r2 


TREESEARCHOBJS	= ../treesearch/SearchManager.o 
				  
NAUTYOBJS     	= ../nauty24r2/nauty.o 			\
				  ../nauty24r2/nausparse.o		\
				  ../nauty24r2/gtools.o			\
				  ../nauty24r2/nautil.o			\
				  ../nauty24r2/naugraph.o
					
LIBOBJS			= $(TREESEARCHOBJS) $(NAUTYOBJS)
			
			
.SUFFIXES: .c .cpp .o .obj .exe 

all: $(OBJECTS) $(TESTS) $(TARGETS)


# The default object compiler
.c.o: $<
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
        
.cpp.o: $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
        
.cpp.exe: $< $(OBJECTS)  
	$(CXX) $(LFLAGS)			\
        	$(INCLUDES)	$(DEBUG)			\
        	$(LIBOBJS) $(LIBS)				\
        	`cat $@.objs`           		\
            $< -o $@
        
.c.exe: $< $(COBJECTS)
	$(CC) 	$(LFLAGS)			    \
        	$(INCLUDES)				\
        	$(NAUTYOBJS)  $(COBJECTS) $(LIBS)		\
            $< -o $@
        
clean:
	rm $(OBJECTS) $(TARGETS) $(TESTS)
	
cleanexe:
	rm $(TARGETS)

clexe:
	rm $(TARGETS)
