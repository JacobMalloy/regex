#ifndef __INTERNAL_REGEX_H__
#define __INTERNAL_REGEX_H__


#include "regex.h"
#include <stdint.h>

enum ast_node_type{
    ast_node_type_string,
    ast_node_type_character,
    ast_node_type_or,
    ast_node_type_range,
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
            struct ast_node **or_data;
            size_t or_length;
            size_t or_size;
        };
        struct{
            uint64_t character_characters[4];
        };
        struct{
            size_t range_low;
            size_t range_high;
            struct ast_node *range_child;
        };
    };
};




// AST COMPILER

struct ast_node *compile_to_ast(char *string);
size_t count_ast_nodes(struct ast_node *head);
void free_ast(struct ast_node *head);



struct ast_node *ast_create_or_node();
void ast_add_to_or_node(struct ast_node *node,struct ast_node *child);


#endif //__INTERNAL_REGEX_H__
