#include "internal_regex.h"
#include <stdio.h>


void free_ast(struct ast_node *head){
    switch(head->type_tag){
        case ast_node_type_character:
        break;
        case ast_node_type_or:
            for(size_t i = 0;i < head->or_length;i++){
                free_ast(head->or_data[i]);
            }
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
        case ast_node_type_or:
            for(size_t i = 0;i < head->or_length;i++){
                count += count_ast_nodes(head->or_data[i]);
            }
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


struct ast_node *ast_create_or_node(){
    struct ast_node *return_node;
    return_node = calloc(1,sizeof(struct ast_node));
    return_node->type_tag = ast_node_type_or;
    return_node->or_size = 2;
    return_node->or_data = malloc(return_node->or_size*sizeof(struct ast_node *));
    return return_node;
}
void ast_add_to_or_node(struct ast_node *node,struct ast_node *child){
    if(node->or_size == node->or_length){
        node->or_size *= 2;
        node->or_data = realloc(node->or_data,node->or_size*sizeof(struct ast_node *));
    }
    node->or_data[node->or_length]=child;
    node->or_length += 1;
}











