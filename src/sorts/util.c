#include "global.h"
#include <string.h>

int swaps=0;
int depth=0;
int calls=0;
int overhead=0;

/* Swap ith item offset from base by i with the one offset from base by j,
 * where items have given size */

void swap( void *base, int i, int j, size_t size)
{
	char tmp[size];

	swaps++;

	memcpy((void *)tmp, base+i*size,size);
	memcpy(base+i*size,base+j*size,size);
	memcpy(base+j*size,(void *)tmp,size);
}
