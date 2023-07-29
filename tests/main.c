#include <stdio.h>
#include <stdlib.h>

#include "regex.h"



int main(){
    struct nfa_node *head;
    head = compile("\\\\");
    printf("Success maybe\n");

    char *search = "\\";

//    printf("The string is%s a match\n",complete_match(head,search)?"":" not");
    printf("The string %s a match\n",match(head,search)?"has":"does not have");

    return 0;
}
