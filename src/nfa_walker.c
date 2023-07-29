#include "regex.h"
#include <stdlib.h>
#include <string.h>

struct queue_item{
    struct nfa_node *current_node;
    char *location;
    struct queue_item *next_item;
};

struct queue{
    struct queue_item *head;
    struct queue_item *tail;
};

static struct queue_item *new_queue_item(struct queue *my_queue,struct nfa_node *current_node,char *location){
    struct queue_item *tmp_queue_item;
    tmp_queue_item = malloc(sizeof(struct queue_item));
    if (my_queue->tail){
        my_queue->tail->next_item = tmp_queue_item;
    }
    my_queue->tail = tmp_queue_item;
    tmp_queue_item->current_node = current_node;
    tmp_queue_item->location = location;
    tmp_queue_item->next_item = NULL;
    if(!my_queue->head){
        my_queue->head = tmp_queue_item;
    }
    return tmp_queue_item;
}

static void free_queue(struct queue *my_queue){
    struct queue_item *current;
    struct queue_item *last;
    current = my_queue->head;
    while(current){
        last = current;
        current = current->next_item;
        free(last);
    }
}

static struct queue_item *pop_queue_item(struct queue *my_queue){
    struct queue_item *return_value;
    if(my_queue->head==NULL){
        return NULL;
    }
    return_value = my_queue->head;
    my_queue->head = return_value->next_item;
    return return_value;
}


int complete_match(struct nfa_node *head,char *string){
    struct queue my_queue;
    struct queue_item *current_item;
    my_queue.head=0;

    new_queue_item(&my_queue,head,string);
    while((current_item = pop_queue_item(&my_queue))){
        char c =  *(current_item->location);
        for(int i=0;i<current_item->current_node->path_list_length;i++){
            if(GET_IN_BITFIELD(current_item->current_node->data[i].characters,0)){
                new_queue_item(&my_queue,current_item->current_node->data[i].next_node,current_item->location);
            }
            if(GET_IN_BITFIELD(current_item->current_node->data[i].characters,c) && c != '\0'){
                new_queue_item(&my_queue,current_item->current_node->data[i].next_node,current_item->location+1);
            }
        }
        if(c == '\0' && current_item->current_node->accept){
            free_queue(&my_queue);
            free(current_item);
            return 1;
        }
        free(current_item);
    }
    return 0;
}



int match(struct nfa_node *head,char *string){
    struct queue my_queue;
    struct queue_item *current_item;
    struct nfa_node head_node;
    struct nfa_path head_path[2];
    my_queue.head=0;

    head_node.path_list_length = 2;
    head_node.path_list_size = 0;
    head_node.data = (struct nfa_path *)&head_path;
    head_node.accept=0;

    SET_IN_BITFIELD(head_path[0].characters,0);
    memset(&head_path[1].characters,0xff,sizeof(head_path[1].characters));
    head_path[1].characters[0] &= ~(uint64_t)1;

    head_path[0].next_node = head;
    head_path[1].next_node = &head_node;


    new_queue_item(&my_queue,&head_node,string);
    while((current_item = pop_queue_item(&my_queue))){
        char c =  *(current_item->location);
        for(int i=0;i<current_item->current_node->path_list_length;i++){
            if(GET_IN_BITFIELD(current_item->current_node->data[i].characters,0)){
                new_queue_item(&my_queue,current_item->current_node->data[i].next_node,current_item->location);
            }
            if(GET_IN_BITFIELD(current_item->current_node->data[i].characters,c) && c != '\0'){
                new_queue_item(&my_queue,current_item->current_node->data[i].next_node,current_item->location+1);
            }
        }
        if(current_item->current_node->accept){
            free_queue(&my_queue);
            free(current_item);
            return 1;
        }
        free(current_item);
    }
    return 0;
}
