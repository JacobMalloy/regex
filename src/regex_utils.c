#include "regex.h"

#include <stdlib.h>
#include <stdio.h>


struct node_list{
    struct nfa_node **data;
    size_t length;
    size_t size;
};

static void dfa(struct nfa_node *node,void (*function_to_do)(struct nfa_node *,void *),void * pass_along,char visited_truthy){
    if (node->visited != visited_truthy){
        function_to_do(node,pass_along);
        node->visited = visited_truthy;
        for (size_t i = 0;i<node->path_list_length;i++){
            dfa(node->data[i].next_node,function_to_do,pass_along,visited_truthy);
        }
    }
}

static void add_node_to_list(struct nfa_node *node,void *void_list){
    struct node_list *list;
    list = (struct node_list *)void_list;
    if(list->length >= list->size){
        list->size *= 2;
        list->data = realloc(list->data,list->size * sizeof(struct nfa_node *));
    }
    list->data[list->length] = node;
    list->length += 1;
}


void regex_free(struct nfa_node *path){
    struct node_list free_list;
    free_list.length = 0;
    free_list.size = 10;
    free_list.data = malloc(free_list.size*sizeof(struct nfa_node *));

    dfa(path,add_node_to_list,(void *)&free_list,!(path->visited));

    for (size_t i = 0;i<free_list.length;i++){
        free(free_list.data[i]->data);
        free(free_list.data[i]);
    }
    free(free_list.data);
}
