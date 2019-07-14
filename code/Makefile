
CCFLAGS = -std=c++11 -O2 -fopenmp
LOADLIBES = -lm

SRCS = Graph.cpp SSSP.cpp ReadWrite.cpp timer.cpp main.cpp
SRCSP = Graph.cpp ReadWrite.cpp timer.cpp main_mpi.cpp
HDRS = Graph.h SSSP.h ReadWrite.h
CODES = serial serial_checksum parallel parallel_checksum

all: $(CODES)

serial: $(SRCS) $(HDRS)
	mpic++ $(CCFLAGS) -o SSSP_serial.exe $(SRCS) $(LOADLIBES)

parallel: $(SRCSP) $(HDRS)
	mpic++ $(CCFLAGS) -o SSSP_parallel.exe $(SRCSP) $(LOADLIBES)

serial_checksum: $(SRCS) $(HDRS)
	mpic++ $(CCFLAGS) -DCHECKSUM -o SSSP_serial_checksum.exe $(SRCS) $(LOADLIBES)

parallel_checksum: $(SRCSP) $(HDRS)
	mpic++ $(CCFLAGS) -DCHECKSUM -o SSSP_parallel_checksum.exe $(SRCSP) $(LOADLIBES)

clean:
	rm -f SSSP_serial.exe SSSP_serial_checksum.exe SSSP_parallel.exe SSSP_parallel_checksum.exe
