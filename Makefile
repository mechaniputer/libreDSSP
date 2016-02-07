all: dssp
dssp: dict.o corewords.o util.o
clean:
	rm -f dssp *.o
