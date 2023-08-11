#ifdef BACKTRACKING_NFA
#include "internal_regex.h"
#include "backtracking_nfa.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ERROR(x) {fprintf(stderr,"AST->NFA Error:%s",x);exit(1);}

struct expression{
    struct nfa_node *start;
    struct nfa_node *end;
};

struct text_info{
    char *start;
    char *location;
};


struct expression compile_ast_to_nfa(struct ast_node *ast);



static struct nfa_node *new_nfa_node(){
    struct nfa_node *return_value;
    return_value = malloc(sizeof(struct nfa_node));
    memset(return_value,0,sizeof(struct nfa_node));
    return_value->path_list_size = 2;
    return_value->data = malloc(return_value->path_list_size*sizeof(struct nfa_path));
    return return_value;
}

struct nfa_path *add_nfa_path(struct nfa_node *node,struct nfa_node *next_node){
    struct nfa_path *return_value;
    if (node->path_list_length == node->path_list_size){
        node->path_list_size*=2;
        node->data = realloc(node->data,node->path_list_size*sizeof(struct nfa_path));
    }
    return_value = &(node->data[node->path_list_length]);
    memset(&(return_value->characters),0,sizeof(return_value->characters));
    return_value->next_node = next_node;
    node->path_list_length += 1;
    return return_value;
}

struct nfa_path *add_nfa_path_epsilon(struct nfa_node *node,struct nfa_node *next_node){
    struct nfa_path *return_value;

    return_value = add_nfa_path(node,next_node);
    SET_IN_BITFIELD(return_value->characters,0)
    return return_value;
}


static struct expression dispatch_character(struct ast_node *ast){
    struct expression return_value;
    struct nfa_path *path;
    return_value.start = new_nfa_node();
    return_value.end = new_nfa_node();
    path = add_nfa_path(return_value.start,return_value.end);
    COPY_TO_BITFIELD(path->characters,ast->character_characters)
    return return_value;
}


static struct expression dispatch_or(struct ast_node *ast){
    struct expression b1,return_value;
    return_value.start = new_nfa_node();
    return_value.end = new_nfa_node();
    for(size_t i = 0;i < ast->or_length; i++){
        b1 = compile_ast_to_nfa(ast->or_data[i]);
        add_nfa_path_epsilon(return_value.start,b1.start);
        add_nfa_path_epsilon(b1.end,return_value.end);
    }
    return return_value;
}

static struct expression dispatch_range(struct ast_node *ast){
    struct expression b1,return_value;
    return_value.start = new_nfa_node();
    return_value.end = new_nfa_node();
    b1 = compile_ast_to_nfa(ast->range_child);
    if(ast->range_low == 0 && ast->range_high == 0){
        add_nfa_path_epsilon(return_value.start,b1.start);
        add_nfa_path_epsilon(b1.end,return_value.end);
        add_nfa_path_epsilon(return_value.start,return_value.end);
        add_nfa_path_epsilon(b1.end,b1.start);
    }else if(ast->range_low == 0 && ast->range_high == 1){
        add_nfa_path_epsilon(return_value.start,b1.start);
        add_nfa_path_epsilon(b1.end,return_value.end);
        add_nfa_path_epsilon(return_value.start,return_value.end);
    }else if(ast->range_low == 1 && ast->range_high == 0){
        add_nfa_path_epsilon(return_value.start,b1.start);
        add_nfa_path_epsilon(b1.end,return_value.end);
        add_nfa_path_epsilon(b1.end,b1.start);
    }else{
        ERROR("Unhandled dispatch range");
    }

    return return_value;
}

static struct expression dispatch_combiner(struct ast_node *ast){
    struct expression return_value,left,right;
    left = compile_ast_to_nfa(ast->combiner_left);
    right = compile_ast_to_nfa(ast->combiner_right);
    add_nfa_path_epsilon(left.end,right.start);
    return_value.start = left.start;
    return_value.end = right.end;
    return return_value;
}

struct expression compile_ast_to_nfa(struct ast_node *ast){
    switch (ast->type_tag){
        case ast_node_type_character:
            return dispatch_character(ast);
        break;
        case ast_node_type_or:
            return dispatch_or(ast);
        break;
        case ast_node_type_range:
            return dispatch_range(ast);
        break;
        case ast_node_type_combiner:
            return dispatch_combiner(ast);
        break;
        default:
            ERROR("Unhandled ast node type")
        break;
    }
}

regex_t compile(char *string){
    struct ast_node *ast;
    struct expression return_value;

    ast = compile_to_ast(string);

    return_value = compile_ast_to_nfa(ast);
    return_value.end->accept=1;

    free_ast(ast);


    return (regex_t)return_value.start;

}


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


void nfa_free(struct nfa_node *path){
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


void regex_free(regex_t regex_info){
    struct nfa_node *node;
    node = (struct nfa_node *)regex_info;
    nfa_free(node);
}

#undef ERROR
#endif  //BACKTRACKING_NFA
