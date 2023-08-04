#ifndef __REGEX_H__
#define __REGEX_H__
#include <stdint.h>
#include <stdlib.h>

#define SET_IN_BITFIELD(f,x) {int x_new = x;f[x_new/64] |= ((uint64_t)1<<(x_new%64));}
#define COPY_TO_BITFIELD(f,x) {memmove(f,x,sizeof(f));}
#define GET_IN_BITFIELD(f,x) ({int x_new = x;(f[x_new/64]>>(x_new%64))&1;})

typedef void * regex_t;


regex_t compile(char *string);
void regex_free(regex_t);
int match(regex_t head,char *string);


#endif //__REGEX_H__
