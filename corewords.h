#ifndef COREWORDS_H
#define COREWORDS_H

#include <stdlib.h>
#include "elem.h"

elem * plus(elem * stack, elem * sequence);
elem * minus(elem * stack, elem * sequence);
elem * bye(elem * stack, elem * sequence);
elem * showTop(elem * stack, elem * sequence);
elem * showStack(elem * stack, elem * sequence);
elem * ifplus(elem * stack, elem * sequence);
elem * ifzero(elem * stack, elem * sequence);
elem * ifminus(elem * stack, elem * sequence);

#endif
