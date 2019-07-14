#include "ReadWrite.h"
#include <stdlib.h>
#include <vector>
using namespace std;

void read_graph(char *fileName, Graph *&graph) {

	int node_num, edge_num;
	char pr_type[3];
	int *from, *to, *weight;
	from = new int[MAXEDGENUM];
	to = new int[MAXEDGENUM];
	weight = new int[MAXEDGENUM];
	int edge_index = 0;

	char *input = new char[MAXFILESIZE];
	if (input == NULL) {
		fprintf(stderr, "ERROR: Memory allocation failure\n");
		exit(1);
	}
	FILE *f = fopen(fileName, "r");
	if (f == NULL) {
		fprintf(stderr, "ERROR: file %s not found\n", fileName);
		exit(1);
	}

	int read_size = fread(input, 1, MAXFILESIZE, f);
	vector<int> line_index;
	line_index.push_back(-1);
	for (int i = 0; i < read_size; i++)
		if (input[i] == '\n')
			line_index.push_back(i);
	int line_num = line_index.size() - 1;
//#pragma omp parallel for
	for (int i = 0; i < line_num; i++) {
		int begin = line_index[i] + 1, end = line_index[i + 1];

		int f, t, w, j, intindex;
		char intbuffer[20];
		switch (input[begin]) {
			case 'c':
			case '\n':
			case '\0':
				break;
			case 'p':
			//	sscanf(input + begin, "%*c %2s %d %d", pr_type, &node_num, &edge_num);
				j = 0;
				intindex = begin + 5;
				while (input[intindex] != ' ') {
					intbuffer[j++] = input[intindex++];
				}
				intbuffer[j] = 0;
				node_num = atoi(intbuffer);
				j = 0;
				intindex++;
				while (input[intindex] != '\n') {
					intbuffer[j++] = input[intindex++];
				}
				intbuffer[j] = 0;
				edge_num = atoi(intbuffer);
				break;
			case 'a':
				j = 0;
				intindex = begin + 2;
				while (input[intindex] != ' ') {
					intbuffer[j++] = input[intindex++];
				}
				intbuffer[j] = 0;
				f = atoi(intbuffer);
				j = 0;
				intindex++;
				while (input[intindex] != ' ') {
					intbuffer[j++] = input[intindex++];
				}
				intbuffer[j] = 0;
				t = atoi(intbuffer);
				j = 0;
				intindex++;
				while (input[intindex] != '\n') {
					intbuffer[j++] = input[intindex++];
				}
				intbuffer[j] = 0;
				w = atoi(intbuffer);
			//	sscanf(input + begin, "%*c %d %d %d", &f, &t, &w);
//#pragma omp critical
				{
					from[edge_index] = f;
					to[edge_index] = t;
					weight[edge_index] = w;
					edge_index++;
				}
				break;
			default: ;
		}
	}

	graph = new Graph(node_num, edge_num, from, to, weight);

	delete[] from;
	delete[] to;
	delete[] weight;
	delete[] input;
}

void read_ss(char *fileName, int &snum, int *&ss) {
#define MAXLINE 100
	int n, s_index = 0, source;
	int *sources = NULL;
	char prA_type[4], pr_type[3], pr_var[3], in_line[MAXLINE];
	FILE *aFile;

	aFile = fopen(fileName, "r");
	if (aFile == NULL) {
		fprintf(stderr, "ERROR: file %s not found\n", fileName);
		exit(1);
	}

	while (fgets(in_line, MAXLINE, aFile) != NULL) {
		switch (in_line[0]) {
			case 'c':
			case '\n':
			case '\0':
				break;
			case 'p':
				sscanf(in_line, "%*c %s %s %s %d", prA_type, pr_type, pr_var, &n);
				sources = new int[n];
				break;
			case 's':
				sscanf(in_line, "%*c %d", &source);
				sources[s_index++] = source;
				break;
			default: ;
		}
	}

	snum = n;
	ss = sources;
}

