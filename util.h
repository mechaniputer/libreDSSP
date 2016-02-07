#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "dict.h"
#include "elem.h"

int isnum(char * foo);
elem * run(elem * stack, elem * seqHead, dict * vocab);
elem * parseInput(char * line);

#endif
