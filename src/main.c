#include <stdio.h>
#include <stdlib.h>

#include "regex.h"



int main(int argc, char **argv){
    struct nfa_node *head;
    head = compile("bad?c");
    printf("Success maybe\n");

    char *search = "baddc";

    printf("The string is%s a match\n",complete_match(head,search)?"":" not");
    printf("The string %s a match\n",match(head,search)?"has":"does not have");

    return 0;
}
