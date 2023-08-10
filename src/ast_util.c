#include "internal_regex.h"
#include <stdio.h>


void free_ast(struct ast_node *head){
    switch(head->type_tag){
        case ast_node_type_character:
        break;
        case ast_node_type_binary:
            free_ast(head->binary_left);
            free_ast(head->binary_right);
        break;
        case ast_node_type_unary:
            free_ast(head->unary_child);
        break;
        case ast_node_type_combiner:
            free_ast(head->combiner_left);
            free_ast(head->combiner_right);
        break;
        case ast_node_type_range:
            free_ast(head->range_child);
        break;
        case ast_node_type_string:
        default:
            fprintf(stderr,"ERROR: Unimplemented free for ast node type");
        break;
    }
    free(head);
}

size_t count_ast_nodes(struct ast_node *head){
    size_t count = 0;
    switch(head->type_tag){
        case ast_node_type_character:
            return 1;
        break;
        case ast_node_type_binary:
            count += count_ast_nodes(head->binary_left);
            count += count_ast_nodes(head->binary_right);
        break;
        case ast_node_type_unary:
            count += count_ast_nodes(head->unary_child);
        break;
        case ast_node_type_combiner:
            count += count_ast_nodes(head->combiner_left);
            count += count_ast_nodes(head->combiner_right);
        break;
        case ast_node_type_range:
            count += count_ast_nodes(head->range_child);
        break;
        case ast_node_type_string:
            return 1;
        default:
            fprintf(stderr,"ERROR: Unimplemented free for ast node type");
        break;
    }
    count += 1;
    return count;
}
