# Parallel-SSSP-MPI
Course project for Introduction to Parallel and Distributed Computing

## Description
In this project, we study the parallelization of SSSP, an important task in graph problems. We investigate Δ stepping algorithm and its further optimization, and implement the algorithms in distributed memory systems, using MPI interface. For the given six datasets, the optimized parallel algorithm implementation could reach about 4 times speedup compared with sequential implementation under the best condition. Though in most experiment cases the implement may not reach the speed of sequential benchmark, it shows that the larger the graph, the better the performance of our algorithm, which may result from the advantage of distributed computing. This indicates that our algorithm implementation could handle large scale graph problem.

## Main reference paper
Chakaravarthy, V. T., Checconi, F., Murali, P., Petrini, F., & Sabharwal, Y. (2016). Scalable single source shortest path algorithms for massively parallel systems. IEEE Transactions on Parallel and Distributed Systems, 28(7), 2031 2045.   
Meyer, U., & Sanders, P. (2003). Δ stepping: a parallelizable shortest path algorithm. Journal of Algorithms, 49(1), 114 152.

