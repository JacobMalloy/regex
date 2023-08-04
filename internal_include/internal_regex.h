#ifndef __INTERNAL_REGEX_H__
#define __INTERNAL_REGEX_H__


#include "regex.h"
#include <stdint.h>

enum ast_node_type{
    ast_node_type_string,
    ast_node_type_character,
    ast_node_type_binary,
    ast_node_type_unary,
    ast_node_type_combiner,
};

struct ast_node{
    enum ast_node_type type_tag;
    struct {
        size_t group_start:1;
    };
    union{
        struct{
            char * string_value;
        };
        struct{
            struct ast_node *combiner_left;
            struct ast_node *combiner_right;
        };
        struct{
            struct ast_node *binary_left;
            struct ast_node *binary_right;
            size_t binary_operator;
        };
        struct{
            struct ast_node *unary_child;
            size_t unary_operator;
        };
        struct{
            uint64_t character_characters[4];
        };
    };
};




// AST COMPILER

struct ast_node *compile_to_ast(char *string);
size_t count_ast_nodes(struct ast_node *head);
void free_ast(struct ast_node *head);


#endif //__INTERNAL_REGEX_H__
