# Parallel-SSSP-MPI
Course project for Introduction to Parallel and Distributed Computing

## Description
In this project, we study the parallelization of SSSP, an important task in graph problems. We investigate Δ stepping algorithm and its further optimization, and implement the algorithms in distributed memory systems, using MPI interface. For the given six datasets, the optimized parallel algorithm implementation could reach about 4 times speedup compared with sequential implementation under the best condition. Though in most experiment cases the implement may not reach the speed of sequential benchmark, it shows that the larger the graph, the better the performance of our algorithm, which may result from the advantage of distributed computing. This indicates that our algorithm implementation could handle large scale graph problem.

## Main reference paper
Chakaravarthy, V. T., Checconi, F., Murali, P., Petrini, F., & Sabharwal, Y. (2016). Scalable single source shortest path algorithms for massively parallel systems. IEEE Transactions on Parallel and Distributed Systems, 28(7), 2031 2045.   
Meyer, U., & Sanders, P. (2003). Δ stepping: a parallelizable shortest path algorithm. Journal of Algorithms, 49(1), 114 152.

## Datasets
USA-road-d, USA-road-t

### Run Serial program
./bin/SSSP_serial.exe \<graph file\> \<aux file\> \<out file\> \<delta\> \<MaxPathLength\>  
where the last two argument would be default by program if set to 0.  
Example: ./bin/SSSP_serial.exe ./input/graph.gr ./input/aux.ss ./result/out.ss 0 0  
If compute checksum, run SSSP_serial_checksum.exe

### Run Parallel program
mpirun -np <core_number> ./bin/SSSP_parallel.exe \<graph file\> \<aux file\> \<out file\> \<delta\> \<MaxPathLength\>  
where the last two argument would be default by program if set to 0.  
Example: mpirun -np 16 ./bin/SSSP_serial.exe ./input/graph.gr ./input/aux.ss ./result/out.ss 0 0  
If compute checksum, run SSSP_serial_checksum.exe  
Core number 16 is better on small datasets like NY and NE, while 20 is better on larger datasets like CTR.
