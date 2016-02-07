all: dssp
dssp: dict.o corewords.o
clean:
	rm -f dssp *.o
