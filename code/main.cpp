#include "Graph.h"
#include "SSSP.h"
#include "ReadWrite.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <algorithm>

using namespace std;

#define MODUL ((long long) 1 << 62)

extern double timer();

int main(int argc, char **argv) {
	double tm = 0.0;

	char gName[100], aName[100], oName[100];
	FILE *oFile;
	Graph *graph = NULL;
	int nQ;
	int *sources = NULL;

	if (argc != 6) {
		fprintf(stderr, "Usage: \"%s <graph file> <aux file> <out file> delta MaxPathLength\"\nMaxPathLength is an upper bound, default is MaxEdgeWeight * sqrt(node_num^2 / edge_num), if the argument is 0", argv[0]);
		exit(0);
	}

	int delta, MAXLEN, bucket_num;
	delta = atoi(argv[4]);
	MAXLEN = atoi(argv[5]);

	strcpy(gName, argv[1]);
	strcpy(aName, argv[2]);
	strcpy(oName, argv[3]);
	oFile = fopen(oName, "a");

	read_graph(gName, graph);
	read_ss(aName, nQ, sources);

	if (delta == 0)
		delta = graph->max_len / 10;
	if (MAXLEN == 0)
		MAXLEN = (int)(graph->max_len * sqrt(graph->node_num * (double)graph->node_num / graph->edge_num));
	bucket_num = MAXLEN / delta;

	fprintf(oFile, "f %s %s\n", gName, aName);

	long long *result = new long long[graph->node_num + 1];

	printf("graph node:%d, edge:%d, len max:%d, min:%d\n",graph->node_num, graph->edge_num, graph->max_len, graph->min_len);
	tm = timer();
	for (int i = 0; i < nQ; i++) {
		int source = sources[i];
		delta_stepping *d = new delta_stepping(delta, graph, source, graph->node_num, result);
#ifdef PARALLEL
		d->parallel_run(bucket_num);
#else
		d->serial_run(bucket_num);
#endif

#ifdef CHECKSUM
		long long dist = 0;
		for (int i = 1; i <= graph->node_num; i++)
			dist = (dist + result[i] % MODUL) % MODUL;

		fprintf(oFile, "d %lld\n", dist);
#endif

		delete d;
	}
	tm = (timer() - tm);

	fprintf(oFile, "t %f\n", 1000.0 * tm / (float)nQ);
	printf("time: %f, source num: %d, ave: %f\n", tm, nQ, 1000.0 * tm / (float)nQ);

	delete[] sources;
	delete[] result;
	delete graph;
	fclose(oFile);

	return 0;
}
