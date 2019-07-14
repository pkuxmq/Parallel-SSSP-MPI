#include "Graph.h"
#include <string.h>
#include <cmath>
#include <algorithm>
#include <stdio.h>
using namespace std;

bool cmp(const Edge &a, const Edge &b) {
	if (a.from < b.from)
		return true;
	else if (a.from > b.from)
		return false;
	else if (a.to < b.to)
		return true;
	else
		return false;
}

Edges::Edges(int n, int e) {
	node_num = n;
	edge_num = e;
	edge_index = 0;
	edge = new Edge[e + 1];
	head = new int[n + 2];
	memset(head, 0, (n + 1) * sizeof(int));
}

Edges::~Edges() {
	delete[] edge;
	delete[] head;
}

void Edges::add_edge(int from, int to, int w) {
	edge[edge_index].from = from;
	edge[edge_index].to = to;
	edge[edge_index].weight = w;
	edge_index++;
}

void Edges::sort() {
	std::sort(edge, edge + edge_index, cmp);
	int h = edge[0].from;
	for (int i = 0; i < edge_index; i++) {
		if (edge[i].from != h) {
			for (int j = h + 1; j <= edge[i].from; j++)
				head[j] = i;
			h = edge[i].from;
		}
	}
	for (int j = h + 1; j <= node_num + 1; j++)
		head[j] = edge_index;
}

Graph::Graph(int node_n, int edge_n) {
	node_num = node_n;
	edge_num = edge_n;
	graph_edges = new Edges(node_n, edge_n);
	degree = new int[node_num];
	max_len = -1;
	min_len = -1;
}

Graph::Graph(int node_n, int edge_n, int *from, int *to, int *weight) {
	node_num = node_n;
	edge_num = edge_n;
	graph_edges = new Edges(node_n, edge_n);
	degree = new int[node_num];
	memset(degree, 0, node_num * sizeof(int));
	max_len = 0;
	min_len = 0x7fffffff;
	for (int i = 0; i < edge_n; i++) {
		graph_edges->add_edge(from[i], to[i], weight[i]);
		degree[from[i]]++;
		max_len = max(max_len, weight[i]);
		min_len = min(min_len, weight[i]);
	}
	graph_edges->sort();
}

Graph::Graph(int node_n, int edge_n, int *from, int *to, int *weight, int node_b, int node_e) {
	node_num = node_n;
	edge_num = edge_n;
	graph_edges = new Edges(node_n, edge_n);
	degree = new int[node_num];
	memset(degree, 0, node_num * sizeof(int));
	max_len = 0;
	min_len = 0x7fffffff;
	for (int i = 0; i < edge_n; i++) {
		if (from[i] < node_b || from[i] > node_e)
			continue;
		graph_edges->add_edge(from[i], to[i], weight[i]);
		degree[from[i]]++;
		max_len = max(max_len, weight[i]);
		min_len = min(min_len, weight[i]);
	}
	graph_edges->sort();
}

Graph::~Graph() {
	delete graph_edges;
}

void Graph::add_edge(int from, int to, int w) {
	graph_edges->add_edge(from, to, w);
}

