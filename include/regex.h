#ifndef __REGEX_H__
#define __REGEX_H__
#include <stdint.h>
#include <stdlib.h>

#define SET_IN_BITFIELD(f,x) {int x_new = x;f[x_new/64] |= ((uint64_t)1<<(x_new%64));}
#define COPY_TO_BITFIELD(f,x) {memmove(f,x,sizeof(f));}
#define GET_IN_BITFIELD(f,x) ({int x_new = x;(f[x_new/64]>>(x_new%64))&1;})

struct nfa_path{
    struct nfa_node *next_node;
    uint64_t characters[4];
};

struct nfa_node{
    size_t path_list_length;
    size_t path_list_size;
    struct nfa_path *data;
    struct{
        uint64_t accept : 1;
        uint64_t visited : 1;
        uint64_t start_group : 1;
        uint64_t end_group : 1;
    };
};

struct nfa_node *compile(char *string);
void regex_free(struct nfa_node *path);
int complete_match(struct nfa_node *head,char *string);
int match(struct nfa_node *head,char *string);


#endif //__REGEX_H__
