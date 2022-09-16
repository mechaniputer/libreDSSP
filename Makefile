#	This file is part of libreDSSP.

#	Copyright 2019 Alan Beadle

#	libreDSSP is free software: you can redistribute it and/or modify \
	it under the terms of the GNU General Public License as published by \
	the Free Software Foundation, either version 3 of the License, or \
	(at your option) any later version.

#	libreDSSP is distributed in the hope that it will be useful, \
	but WITHOUT ANY WARRANTY; without even the implied warranty of \
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
	GNU General Public License for more details. \

#	You should have received a copy of the GNU General Public License \
	along with libreDSSP.  If not, see <http://www.gnu.org/licenses/>.
CFLAGS = -I/usr/local/include -L/usr/local/lib -Wall -ggdb
LDLIBS = -lreadline
all: dssp
dssp: stack.o cmdbuf.o dict.o corewords.o util.o
	cc $(CFLAGS) dssp.c -o dssp stack.o cmdbuf.o dict.o corewords.o util.o $(LDLIBS)
stack.o: stack.c stack.h
cmdbuf.o: cmdbuf.c cmdbuf.h
dict.o: dict.c dict.h
corewords.o: corewords.c corewords.h
util.o: util.c util.h
clean:
	rm -f dssp *.o
.c.o:
	cc $(CFLAGS) -c $<
