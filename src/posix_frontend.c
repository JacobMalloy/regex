#include "internal_regex.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PARSE_ERROR(x) {fprintf(stderr,"PARSE_ERROR: %s\n",x);exit(1);}


struct text_info{
    char *start;
    char *location;
};


//parser steps
static struct ast_node *expression(struct text_info *text_info);
static struct ast_node *atom(struct text_info *text_info);
static struct ast_node *group(struct text_info *text_info);
static struct ast_node *character_group(struct text_info *text_info);


static char get_escaped_character(char ** location);
static struct ast_node *new_ast_node();



static struct ast_node *new_ast_node(){
    struct ast_node *return_node;
    return_node = malloc(sizeof(struct ast_node));
    memset(return_node,0,sizeof(struct ast_node));
    return return_node;
}

static char get_escaped_character(char ** location){
    char return_char;
    if(**location == '\\'){
        return_char = *((*location)+1);
        (*location)+=1;
    }else{
        return_char = **(location);
    }
    (*location)+=1;
    return return_char;
}

static struct ast_node *character_group(struct text_info *text_info){
    struct ast_node *return_value;
    return_value = new_ast_node();
    return_value->type_tag = ast_node_type_character;
    if (*(text_info->location) == '['){
        text_info->location += 1;
        while (*(text_info->location) && *(text_info->location)!=']'){
            char c,c2;
            c=get_escaped_character(&text_info->location);
            if (*(text_info->location) == '-'){
                text_info->location += 1;
                c2 = get_escaped_character(&text_info->location);
                for(int i = c;i<=c2;i++){
                    SET_IN_BITFIELD(return_value->character_characters,i)
                }
            }else{
                SET_IN_BITFIELD(return_value->character_characters,c)
            }
        }
        if (*(text_info->location)!=']'){
            PARSE_ERROR("Expected closing ]");
        }
        text_info->location += 1;
    }else{
        SET_IN_BITFIELD(return_value->character_characters,get_escaped_character(&text_info->location))
    }
    return return_value;
}

static struct ast_node *group(struct text_info *text_info){
    struct ast_node *return_value;
    if (*(text_info->location) == '('){
        text_info->location += 1;
        return_value = expression(text_info);
        return_value->group_start = 1;
        if (*(text_info->location) != ')'){
            PARSE_ERROR("Missing closing )")
        }
        text_info->location += 1;
    }else{
        return_value = character_group(text_info);
    }
    return return_value;
}

static struct ast_node *atom(struct text_info *text_info){
    struct ast_node *return_node, *tmp_node;
    return_node = group(text_info);
    switch (*(text_info->location)){
        case '*':
            tmp_node = new_ast_node();
            tmp_node->type_tag = ast_node_type_range;
            tmp_node->range_child = return_node;
            tmp_node->range_low = 0;
            tmp_node->range_high = 0;
            text_info->location += 1;
            return_node = tmp_node;
        break;
        case '+':
            tmp_node = new_ast_node();
            tmp_node->type_tag = ast_node_type_range;
            tmp_node->range_child = return_node;
            tmp_node->range_low = 1;
            tmp_node->range_high = 0;
            text_info->location += 1;
            return_node = tmp_node;
        break;
        case '?':
            tmp_node = new_ast_node();
            tmp_node->type_tag = ast_node_type_range;
            tmp_node->range_child = return_node;
            tmp_node->range_low = 0;
            tmp_node->range_high = 1;
            text_info->location += 1;
            return_node = tmp_node;
        break;
        case '|':
            tmp_node = new_ast_node();
            tmp_node->type_tag = ast_node_type_binary;
            tmp_node->binary_left = return_node;
            tmp_node->binary_operator = *(text_info->location);
            text_info->location += 1;
            tmp_node->binary_right = group(text_info);
            return_node = tmp_node;
        break;
        default:
        break;
    }

    return return_node;
}

static struct ast_node *expression(struct text_info *text_info){
    struct ast_node *return_node;
    return_node = atom(text_info);
    while(*text_info->location){
        struct ast_node *tmp_node;
        tmp_node = new_ast_node();
        tmp_node->type_tag = ast_node_type_combiner;
        tmp_node->combiner_left = return_node;
        tmp_node->combiner_right = atom(text_info);
        return_node = tmp_node;
    }
    return return_node;
}

struct ast_node *compile_to_ast(char *string){
    struct text_info text_info;
    text_info.start = text_info.location = string;
    return expression(&text_info);
}
