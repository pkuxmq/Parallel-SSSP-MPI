#include "SSSP.h"
#include <vector>
#include <cmath>
#include <atomic>
#include <omp.h>
#include <stdio.h>
#include <string.h>
using namespace std;

long long inf = 0x7fffffffffffffff;

delta_stepping::delta_stepping(int delta, Graph *graph, int s, int node_num, long long *result) {
	this->delta = delta;
	this->graph = graph;
	this->s = s;
	this->node_num = node_num;
	this->result = result;
	B = NULL;
}

delta_stepping::~delta_stepping() {
	if (B != NULL)
		delete[] B;
}

void delta_stepping::bucket_init(int bn) {
	if (B != NULL)
		delete[] B;
	B = new set<int>[bn];
}

void delta_stepping::relax(int v, long long x) {
	if (x < result[v]) {
		int old_index = min((int)(result[v] / delta), bucket_num - 1);
		int new_index = min((int)(x / delta), bucket_num - 1);
		if (result[v] != inf) {
			B[old_index].erase(v);
		}
		B[new_index].insert(v);
		result[v] = x;
		current_max_bn = max(current_max_bn, new_index);
	}
}

void delta_stepping::findRequests(set<int> &vset, vector<pair<int, long long> > &Req, Kind k, int c) {
	Edges *edges = graph->graph_edges;
	if (k == light) {
		for (int v : vset) {
			for (int edge_index = edges->head[v]; edge_index < edges->head[v + 1]; edge_index++) {
				Edge edge = edges->edge[edge_index];
				if (edge.weight <= c - result[v]) {
					Req.push_back(make_pair(edge.to, result[v] + edge.weight));
				}
			}
		}
	}
	else if (k == heavy) {
		for (int v : vset) {
			for (int edge_index = edges->head[v]; edge_index < edges->head[v + 1]; edge_index++) {
				Edge edge = edges->edge[edge_index];
				if (edge.weight > c - result[v]) {
					Req.push_back(make_pair(edge.to, result[v] + edge.weight));
				}
			}
		}
	}
	else {
		for (int v : vset) {
			for (int edge_index = edges->head[v]; edge_index < edges->head[v + 1]; edge_index++) {
				Edge edge = edges->edge[edge_index];
				Req.push_back(make_pair(edge.to, result[v] + edge.weight));
			}
		}
	}
}

void delta_stepping::serial_run(int bucket_num) {
	this->bucket_num = bucket_num;
	bucket_init(bucket_num);
	for (int i = 1; i <= node_num; i++) {
		result[i] = inf;
	}
	relax(s, 0);

	int i = 0;
	current_max_bn = 0;

	while (i <= current_max_bn) {
	//	printf("bucket %d\n", i);
		set<int> R;
		while (!B[i].empty()) {
			vector<pair<int, long long> > Req;
			if (i == this->bucket_num - 1)
				findRequests(B[i], Req, all, (i + 1) * delta - 1);
			else
		   		findRequests(B[i], Req, light, (i + 1) * delta - 1);
			R.insert(B[i].begin(), B[i].end());
			B[i].clear();

			for (auto j : Req) {
				relax(j.first, j.second);
			}
		}
		if (i == this->bucket_num - 1)
			break;
		vector<pair<int, long long> > Req;
		findRequests(R, Req, heavy, (i + 1) * delta - 1);
		for (auto j : Req) {
			relax(j.first, j.second);
		}
		for (; i <= current_max_bn; i++)
			if (!B[i].empty())
				break;
	}
}
/*
void delta_stepping::relax_parallel(int v, long long x) {
	if (x < result[v]) {
		if (result[v] != inf) {
			B[(result[v] / delta) % bucket_num].erase(v);
		}
		B[(x / delta) % bucket_num].insert(v);
		result[v] = x;
	}
}

void delta_stepping::findRequests_parallel(set<int> &vset, vector<pair<int, long long> > &Req, Kind k) {
	Edges *edges = graph->graph_edges;
	if (k == light) {
		vector<int> vv;
		for (auto v : vset)
			vv.push_back(v);
//#pragma omp parallel for
		for (int i = 0; i < vv.size(); i++) {
			int v = vv[i];
			int edge_index = edges->head_light[v];
			while (edge_index != 0) {
				Edge edge = edges->edge[edge_index];
				pair<int, long long> p = make_pair(edge.to, result[v] + edge.weight);
//#pragma omp critical
				Req.push_back(p);
				edge_index = edge.light_next;
			}
		}
	}
	else {
		vector<int> vv;
		for (auto v : vset)
			vv.push_back(v);
//#pragma omp parallel for
		for (int i = 0; i < vv.size(); i++) {
			int v = vv[i];
			int edge_index = edges->head_heavy[v];
			while (edge_index != 0) {
				Edge edge = edges->edge[edge_index];
				pair<int, long long> p = make_pair(edge.to, result[v] + edge.weight);
//#pragma omp critical
				Req.push_back(p);
				edge_index = edge.heavy_next;
			}
		}
	}
}

void delta_stepping::parallel_run(int bucket_num) {
	this->bucket_num = bucket_num;
	bucket_init(bucket_num);
#pragma omp parallel for
	for (int i = 1; i <= node_num; i++) {
		graph->classify_heavy_light(i, delta);
		result[i] = inf;
	}
	relax(s, 0);

	int i;
	for (i = 0; i < bucket_num; i++)
		if (!B[i].empty())
			break;

	while (i < bucket_num) {
		set<int> R;
		while (!B[i].empty()) {
			vector<pair<int, long long> > Req;
//#pragma omp parallel
			{
			findRequests_parallel(B[i], Req, light);
			R.insert(B[i].begin(), B[i].end());
			}
			B[i].clear();
			for (auto j : Req)
				relax_parallel(j.first, j.second);
		}
		vector<pair<int, long long> > Req;
		findRequests_parallel(R, Req, heavy);
		for (auto j : Req)
			relax_parallel(j.first, j.second);

		for (i = 0; i < bucket_num; i++)
			if (!B[i].empty())
				break;
	}
}
*/
