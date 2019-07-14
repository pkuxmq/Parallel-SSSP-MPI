#ifndef __READWRITE__
#define __READWRITE__

#include "Graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>

#define MAXFILESIZE 1000000000
#define MAXEDGENUM 100000000

void read_graph(char *fileName, Graph *&graph);
void read_ss(char *fileName, int &snum, int *&ss);
inline void read_local_graph(char *fileName, Graph *&graph, int rank, int world_size);

#endif
