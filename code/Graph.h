#ifndef __GRAPH__
#define __GRAPH__

class Edge {
public:
	int from;
	int to;
	int weight;
};

class Edges {
public:
	Edge *edge;
	int *head;
	int node_num;
	int edge_num;
	int edge_index;
	
	Edges(int n, int e);
	~Edges();

	void add_edge(int from, int to, int w);
	void sort();
};

class Graph {
public:
	int node_num;
	int edge_num;
	Edges *graph_edges;
	int *degree;
	int max_len, min_len;

	Graph(int node_n, int edge_n);
	Graph(int node_n, int edge_n, int *from, int *to, int *weight);
	Graph(int node_n, int edge_n, int *from, int *to, int *weight, int node_b, int node_e);
	~Graph();

	void add_edge(int from, int to, int w);
};

#endif
