#include "regex.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PARSE_ERROR(x) {fprintf(stderr,"PARSE_ERROR: %s\n",x);exit(1);}





struct expression{
    struct nfa_node *start;
    struct nfa_node *end;
};

struct text_info{
    char *start;
    char *location;
};


static struct expression atom(struct text_info *text_info);

static struct expression expression(struct text_info *text_info);


static struct nfa_node *new_nfa_node(){
    struct nfa_node *return_value;
    return_value = malloc(sizeof(struct nfa_node));
    return_value->path_list_size = 2;
    return_value->path_list_length = 0;
    return_value->accept = 0;
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

static struct expression character_group(struct text_info *text_info){
    struct expression return_value;
    struct nfa_path *tmp_path;
    return_value.start = new_nfa_node();
    return_value.end = new_nfa_node();
    tmp_path = add_nfa_path(return_value.start,return_value.end);
    if (*(text_info->location) == '['){
        text_info->location += 1;
        while (*(text_info->location) && *(text_info->location)!=']'){
            char c,c2;
            c=get_escaped_character(&text_info->location);
            if (*(text_info->location) == '-'){
                text_info->location += 1;
                c2 = get_escaped_character(&text_info->location);
                for(int i = c;i<=c2;i++){
                    SET_IN_BITFIELD(tmp_path->characters,i)
                }
            }else{
                SET_IN_BITFIELD(tmp_path->characters,c)
            }
        }
        if (*(text_info->location)!=']'){
            PARSE_ERROR("Expected closing ]");
        }
        text_info->location += 1;
    }else{
        SET_IN_BITFIELD(tmp_path->characters,get_escaped_character(&text_info->location))
    }
    return return_value;
}

static struct expression group(struct text_info *text_info){
    struct expression return_value;
    if (*(text_info->location) == '('){
        text_info->location += 1;
        return_value = expression(text_info);
        if (*(text_info->location) != ')'){
            PARSE_ERROR("Missing closing )")
        }
        text_info->location += 1;
    }else{
        return_value = character_group(text_info);
    }
    return return_value;
}

static struct expression atom(struct text_info *text_info){
    struct expression b1,b2,return_value;
    b1 = group(text_info);
    switch (*(text_info->location)){
        case '*':
            text_info->location += 1;
            return_value.start = new_nfa_node();
            return_value.end = new_nfa_node();
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(b1.end,b1.start);
            add_nfa_path_epsilon(return_value.start,return_value.end);
        break;
        case '+':
            text_info->location += 1;
            return_value.start = new_nfa_node();
            return_value.end = new_nfa_node();
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(b1.end,b1.start);
        break;
        case '?':
            text_info->location += 1;
            return_value.start = new_nfa_node();
            return_value.end = new_nfa_node();
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(return_value.start,return_value.end);
        break;
        case '|':
            text_info->location += 1;
            b2 = group(text_info);
            return_value.start = new_nfa_node();
            return_value.end = new_nfa_node();
            add_nfa_path_epsilon(return_value.start,b1.start);
            add_nfa_path_epsilon(return_value.start,b2.start);
            add_nfa_path_epsilon(b1.end,return_value.end);
            add_nfa_path_epsilon(b2.end,return_value.end);
        break;
        default:
        return_value = b1;
        break;
    }
    return return_value;
}

static struct expression expression(struct text_info *text_info){
    struct expression return_expression;
    return_expression = atom(text_info);
    while(*text_info->location){
        struct expression tmp_expression;
        tmp_expression = atom(text_info);
        add_nfa_path_epsilon(return_expression.end,tmp_expression.start);
        return_expression.end = tmp_expression.end;
    }
    return return_expression;
}

struct nfa_node *compile(char *string){
    struct text_info text_info;
    struct expression return_value;
    text_info.start = text_info.location = string;
    return_value = expression(&text_info);
    return_value.end->accept = 1;
    return return_value.start;
}
