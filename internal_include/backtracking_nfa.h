#ifndef __BACKTRACKING_NFA_H__
#define __BACKTRACKING_NFA_H__
#ifdef BACKTRACKING_NFA

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

#endif //BACKTRACKING_NFA
#endif //__BACKTRACKING_NFA_H__
