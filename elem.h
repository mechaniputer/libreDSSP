#ifndef ELEM_H
#define ELEM_H

typedef struct elem elem;
struct elem
{
	char chars[10];
	int value;
	elem * next;
};

#endif
