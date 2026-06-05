#	This file is part of libreDSSP.

#	Copyright 2026 Alan Beadle

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

# Include the auto-generated configurations
include config.mk

# Core user/developer flags
# We avoid += here for compatibility with illumos Make, which requires that variables be defined before they can be appended to.
BASE_CFLAGS = -I/usr/local/include -Wall -O2
DEBUG =
COMBINED_CFLAGS =  $(CFLAGS) $(BASE_CFLAGS) $(DEBUG)

OBJS = stack.o tokparse.o cmdbuf.o dict.o corewords.o util.o

.PHONY: all debug clean

all: dssp

debug:
	$(MAKE) all DEBUG="-ggdb $(CONFIG_DEBUG_SANITIZERS)"

dssp: dssp.c $(OBJS)
	$(CC) $(COMBINED_CFLAGS) $(CFLAGS_LIB) -L/usr/local/lib dssp.c -o dssp $(OBJS) $(LDLIBS_LIB)

# Explicit header dependencies
stack.o: stack.c stack.h
tokparse.o: tokparse.c tokparse.h
cmdbuf.o: cmdbuf.c cmdbuf.h
dict.o: dict.c dict.h
corewords.o: corewords.c corewords.h
util.o: util.c util.h

clean:
	rm -f dssp *.o config.mk

.SUFFIXES: .c .o
.c.o:
	$(CC) $(COMBINED_CFLAGS) $(CFLAGS_LIB) -c $< -o $@
