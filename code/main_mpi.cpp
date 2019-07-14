#include "Graph.h"
#include "ReadWrite.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <cmath>
#include <set>
#include <vector>

using namespace std;

#define MODUL ((long long) 1 << 62)
#define MAXSENDSIZE 10000
#define inf 0x7fffffffffffffff

extern double timer();

int main(int argc, char **argv) {
	double tm = 0.0;

	char gName[100], aName[100], oName[100];
	FILE *oFile;
	Graph *local_graph = NULL;
	int nQ;
	int *sources = NULL;
	int delta, MAXLEN, bucket_num;

	int world_size, world_rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (argc != 6) {
		fprintf(stderr, "Usage: \"%s <graph file> <aux file> <out file> <delta> <MaxPathLength>\"\nMaxPathLength is an upper bound, default is MaxEdgeLenght * sqrt(node_num * node_num / edge_num), if set the argument 0\n", argv[0]);
		exit(0);
	}

	delta = atoi(argv[4]);
	MAXLEN = atoi(argv[5]);

	strcpy(gName, argv[1]);
	strcpy(aName, argv[2]);
	strcpy(oName, argv[3]);
	if (world_rank == 0)
		oFile = fopen(oName, "a");

	read_local_graph(gName, local_graph, world_rank, world_size);

	if (delta == 0)
		delta = local_graph->max_len / 10;
	if (MAXLEN == 0)
		MAXLEN = (int)(local_graph->max_len * sqrt(local_graph->node_num * (double)local_graph->node_num / local_graph->edge_num));
	bucket_num = MAXLEN / delta;

	int per_node_num = local_graph->node_num / world_size + 1;
	int node_b = world_rank * per_node_num + 1, node_e = (world_rank + 1) * per_node_num;
	if (node_e > local_graph->node_num)
		node_e = local_graph->node_num;

	if (world_rank == 0)
		read_ss(aName, nQ, sources);
	MPI_Bcast(&nQ, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (world_rank != 0)
		sources = new int[nQ];
	MPI_Bcast(sources, nQ, MPI_INT, 0, MPI_COMM_WORLD);

	if (world_rank == 0)
		fprintf(oFile, "f %s %s\n", gName, aName);

	long long *result = new long long[local_graph->node_num + 1];

	int *send_size = new int[world_size];
	int *send_size_all = new int[world_size * world_size];
	int **send_buffer1 = new int *[world_size];
	long long **send_buffer2 = new long long *[world_size];
	int **recv_buffer1 = new int *[world_size];
	long long **recv_buffer2 = new long long *[world_size];
	for (int i = 0; i < world_size; i++) {
		send_buffer1[i] = new int[MAXSENDSIZE];
		send_buffer2[i] = new long long[MAXSENDSIZE];
		recv_buffer1[i] = new int[MAXSENDSIZE];
		recv_buffer2[i] = new long long[MAXSENDSIZE];
	}

	if (world_rank == 0) {
		tm = timer();
		printf("graph node:%d, edge:%d\n", local_graph->node_num, local_graph->edge_num);
	}

	for (int i = 0; i < nQ; i++) {
		int source = sources[i];

		set<int> *B = new set<int>[bucket_num];
		for (int i = node_b; i <= node_e; i++) {
			if (i == source) {
				result[i] = 0;
				B[0].insert(i);
			}
			else
				result[i] = inf;
		}

		int global_k = -1;
		int local_k = -1;
		int bucket_done_flag = 0;
		int local_bucket_done_flag = 0;

		while(global_k < bucket_num) {
			local_k = global_k + 1;
			for (; local_k < bucket_num && B[local_k].empty(); local_k++);

			MPI_Allreduce(&local_k, &global_k, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
			if (global_k >= bucket_num)
				break;
			bucket_done_flag = 0;
			set<int> R;
			while(!B[global_k].empty() || !bucket_done_flag) {
				memset(send_size, 0, world_size * sizeof(int));
				memset(send_size_all, 0, world_size * world_size * sizeof(int));
				vector<pair<int, long long> > Req;

				if (!B[global_k].empty()) {
					for (auto v : B[global_k]) {
						for (int edge_index = local_graph->graph_edges->head[v]; edge_index < local_graph->graph_edges->head[v + 1]; edge_index++) {
							Edge edge = local_graph->graph_edges->edge[edge_index];
							if (edge.weight > (global_k + 1) * delta - 1 - result[v] && global_k != bucket_num - 1) {
								continue;
							}
							int to_rank = (edge.to - 1) / per_node_num;
							if (to_rank == world_rank) {
								Req.push_back(make_pair(edge.to, result[v] + edge.weight));
							}
							else {
								send_buffer1[to_rank][send_size[to_rank]] = edge.to;
								send_buffer2[to_rank][send_size[to_rank]] = result[v] + edge.weight;
								send_size[to_rank]++;
							}
						}
					}
				}

				MPI_Allgather(send_size, world_size, MPI_INT, send_size_all, world_size, MPI_INT, MPI_COMM_WORLD);

				for (int sender = 0; sender < world_size; sender++) {
					for (int receiver = 0; receiver < world_size; receiver++) {
						int sindex = sender * world_size + receiver;
						if ((sender == world_rank) && (receiver != world_rank) && (send_size_all[sindex] > 0)) {
							MPI_Send(send_buffer1[receiver], send_size_all[sindex], MPI_INT, receiver, 0, MPI_COMM_WORLD);
							MPI_Send(send_buffer2[receiver], send_size_all[sindex], MPI_LONG_LONG, receiver, 0, MPI_COMM_WORLD);
						}
						else if ((sender != world_rank) && (receiver == world_rank) && (send_size_all[sindex] > 0)) {
							MPI_Recv(recv_buffer1[sender], send_size_all[sindex], MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							MPI_Recv(recv_buffer2[sender], send_size_all[sindex], MPI_LONG_LONG, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							for (int i = 0; i < send_size_all[sindex]; i++) {
								Req.push_back(make_pair(recv_buffer1[sender][i], recv_buffer2[sender][i]));
							}
						}
					}
				}
				R.insert(B[global_k].begin(), B[global_k].end());
				B[global_k].clear();
				for (auto i : Req) {
					int v = i.first;
					long long x = i.second;
					if (x < result[v]) {
						int old_index = min((int)(result[v] / delta), bucket_num - 1);
						int new_index = min((int)(x / delta), bucket_num - 1);
						if (result[v] != inf) {
							B[old_index].erase(v);
						}
						B[new_index].insert(v);
						result[v] = x;
					}
				}

				if (B[global_k].empty())
					local_bucket_done_flag = 1;
				else
					local_bucket_done_flag = 0;
				MPI_Allreduce(&local_bucket_done_flag, &bucket_done_flag, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
			}

			memset(send_size, 0, world_size * sizeof(int));
			memset(send_size_all, 0, world_size * world_size * sizeof(int));
			vector<pair<int, long long> > Req;
			for (auto v : R) {
				for (int edge_index = local_graph->graph_edges->head[v]; edge_index < local_graph->graph_edges->head[v + 1]; edge_index++) {
					Edge edge = local_graph->graph_edges->edge[edge_index];
					if (edge.weight <= (global_k + 1) * delta - 1 - result[v] || global_k == bucket_num - 1) {
						continue;
					}
					int to_rank = (edge.to - 1) / per_node_num;
					if (to_rank == world_rank) {
						Req.push_back(make_pair(edge.to, result[v] + edge.weight));
					}
					else {
						send_buffer1[to_rank][send_size[to_rank]] = edge.to;
						send_buffer2[to_rank][send_size[to_rank]] = result[v] + edge.weight;
						send_size[to_rank]++;
					}
				}
			}

			MPI_Allgather(send_size, world_size, MPI_INT, send_size_all, world_size, MPI_INT, MPI_COMM_WORLD);

			for (int sender = 0; sender < world_size; sender++) {
				for (int receiver = 0; receiver < world_size; receiver++) {
					int sindex = sender * world_size + receiver;
					if ((sender == world_rank) && (receiver != world_rank) && (send_size_all[sindex] > 0)) {
						MPI_Send(send_buffer1[receiver], send_size_all[sindex], MPI_INT, receiver, 0, MPI_COMM_WORLD);
						MPI_Send(send_buffer2[receiver], send_size_all[sindex], MPI_LONG_LONG, receiver, 0, MPI_COMM_WORLD);
					}
					else if ((sender != world_rank) && (receiver == world_rank) && (send_size_all[sindex] > 0)) {
						MPI_Recv(recv_buffer1[sender], send_size_all[sindex], MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						MPI_Recv(recv_buffer2[sender], send_size_all[sindex], MPI_LONG_LONG, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						for (int i = 0; i < send_size_all[sindex]; i++) {
							Req.push_back(make_pair(recv_buffer1[sender][i], recv_buffer2[sender][i]));
						}
					}
				}
			}
			for (auto i : Req) {
				int v = i.first;
				long long x = i.second;
				if (x < result[v]) {
					int old_index = min((int)(result[v] / delta), bucket_num - 1);
					int new_index = min((int)(x / delta), bucket_num - 1);
					if (result[v] != inf) {
						B[old_index].erase(v);
					}
					B[new_index].insert(v);
					result[v] = x;
				}
			}
		}

		for (int r = 0; r < world_size; r++) {
			if (world_rank != r)
				continue;
			if (r == 0) {
				for (int i = 1; i < world_size; i++) {
					int n_b = i * per_node_num + 1;
					int n_e = min((i + 1) * per_node_num, local_graph->node_num);
					MPI_Recv(result + n_b, n_e - n_b + 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			}
			else {
				MPI_Send(result + node_b, node_e - node_b + 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			}
		}

#ifdef CHECKSUM
		if (world_rank == 0) {
			long long dist = 0;
			for (int i = 1; i <= local_graph->node_num; i++)
				dist = (dist + result[i] % MODUL) % MODUL;

			fprintf(oFile, "d %lld\n", dist);
		}
#endif

		delete[] B;
	}
	if (world_rank == 0) {
		tm = (timer() - tm);

		fprintf(oFile, "t %f\n", 1000.0 * tm / (float)nQ);
		printf("time: %f, source num: %d, ave: %f\n", tm, nQ, 1000.0 * tm / (float)nQ);

		fclose(oFile);
	}

	delete[] send_size;
	delete[] send_size_all;
	for (int i = 0; i < world_size; i++) {
		delete[] send_buffer1[i];
		delete[] send_buffer2[i];
		delete[] recv_buffer1[i];
		delete[] recv_buffer2[i];
	}
	delete[] send_buffer1;
	delete[] send_buffer2;
	delete[] recv_buffer1;
	delete[] recv_buffer2;

	delete[] sources;
	delete[] result;
	delete local_graph;

	MPI_Finalize();

	return 0;
}

inline void read_local_graph(char *fileName, Graph *&graph, int rank, int world_size) {
	int node_num, edge_num;
	char pr_type[3];
	int *from, *to, *weight;
	from = new int[MAXEDGENUM];
	to = new int[MAXEDGENUM];
	weight = new int[MAXEDGENUM];

	int MAXWEIGHT = 0;
	if (rank == 0) {
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
					from[edge_index] = f;
					to[edge_index] = t;
					weight[edge_index] = w;
					MAXWEIGHT = max(MAXWEIGHT, w);
					edge_index++;
					break;
				default: ;
			}
		}
		delete[] input;
	}

	MPI_Bcast(&node_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&edge_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(from, edge_num, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(to, edge_num, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(weight, edge_num, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&MAXWEIGHT, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int per_node_num = node_num / world_size + 1;
	int node_b = rank * per_node_num + 1, node_e = (rank + 1) * per_node_num;
	if (node_e > node_num)
		node_e = node_num;

	graph = new Graph(node_num, edge_num, from, to, weight, node_b, node_e);
	graph->max_len = MAXWEIGHT;

	delete[] from;
	delete[] to;
	delete[] weight;
}
