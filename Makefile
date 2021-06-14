# MPI Compiler
CC=mpicxx
# MPI Options
CFLAGS=-O3 -DNO_FREETYPE
# OPENMPI/MPICH path
MPI=/usr/lib/opempi
# OPENMPI/MPICH library
LMPI=-lmpi

LIBMPI=-L$(MPI)/ $(LMPI)

all:
	$(CC) $(CFLAGS) main.cpp $(LIBMPI) -o osmosempi

clean:
	rm -f osmosempi

	
