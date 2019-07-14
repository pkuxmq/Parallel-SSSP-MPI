#ifndef __SSSP__
#define __SSSP__
#include "Graph.h"
#include <set>
#include <vector>
using namespace std;

typedef enum{light, heavy, all} Kind;

class delta_stepping {
public:
	int delta;
	Graph *graph;
	int s, node_num;
	long long *result;
	set<int> *B;
	int bucket_num;
	int current_max_bn;

	void bucket_init(int bn);

	void relax(int v, long long x);
	void findRequests(set<int> &vset, vector<pair<int, long long> > &Req, Kind k, int c);
	void serial_run(int bucket_num);
	//void relax_parallel(int v, long long x);
	//void findRequests_parallel(set<int> &vset, vector<pair<int, long long> > &Req, Kind k);
	//void parallel_run(int bucket_num);

	delta_stepping(int delta, Graph *graph, int s, int node_num, long long *result);
	~delta_stepping();
};

#endif
