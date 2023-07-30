#include "internal_regex.h"
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


static struct expression dispatch_binary(struct ast_node *ast){
    struct expression b1,b2,return_value;
    return_value.start = new_nfa_node();
    return_value.end = new_nfa_node();
    b1 = compile_ast_to_nfa(ast->binary_left);
    b2 = compile_ast_to_nfa(ast->binary_right);
    switch (ast->binary_operator){
        case '|':
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(return_value.start,b2.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(b2.end,return_value.end);
        break;
        default:
            ERROR("Unhandled binary operator");
        break;
    }
    return return_value;
}

static struct expression dispatch_unary(struct ast_node *ast){
    struct expression b1,return_value;
    return_value.start = new_nfa_node();
    return_value.end = new_nfa_node();
    b1 = compile_ast_to_nfa(ast->unary_child);
    switch (ast->unary_operator){
        case '*':
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(b1.end,b1.start);
            add_nfa_path_epsilon(return_value.start,return_value.end);
        break;
        case '+':
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(b1.end,b1.start);
        break;
        case '?':
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(return_value.start,return_value.end);
        break;
        default:
        ERROR("Unhandled binary operator");
        break;
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
        case ast_node_type_binary:
            return dispatch_binary(ast);
        break;
        case ast_node_type_unary:
            return dispatch_unary(ast);
        break;
        case ast_node_type_combiner:
            return dispatch_combiner(ast);
        break;
        default:
            ERROR("Unhandled ast node type")
        break;
    }
}

struct nfa_node *compile(char *string){
    struct ast_node *ast;
    struct expression return_value;

    ast = compile_to_ast(string);

    return_value = compile_ast_to_nfa(ast);
    return_value.end->accept=1;


    return return_value.start;

}

#undef ERROR
