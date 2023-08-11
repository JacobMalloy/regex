#include "regex.h"

#include <stdio.h>
#include <stdlib.h>


void test(char * regex, char *string, regex_t head, int should_pass){
    int should_free = 0;
    if(!head){
        should_free = 1;
        head = compile(regex);
    }
    if (match(head,string)!=should_pass){
        regex_free(head);
        fprintf(stderr,"Failed!\nstring:%s\nregex:%s\n%s\n\n",string,regex,should_pass?"Did not match":"Matched");
        exit(1);
    }else{
        fprintf(stderr,"Passed!\nstring:%s\nregex:%s\n%s\n\n",string,regex,!should_pass?"Did not match":"Matched");
    }
    if(should_free){
        regex_free(head);
    }
}

int main(){
    test("ab","cab",NULL,1);
    test("abc","cab",NULL,0);
    test("[ab]","a",NULL,1);
    test("[ab]*","",NULL,1);
    test("[ab]*","baba",NULL,1);
    test("[ab]+","baba",NULL,1);
    test("[ab]+","",NULL,0);
    test("[a-zA-Z]+","abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",NULL,1);
    test("[a-z]+","1",NULL,0);
    test("a|b","a",NULL,1);
    test("a|b","c",NULL,0);
    test("a|b|c","c",NULL,1);
    test("a|b|c","c",NULL,1);
    test("(abd)|b|c|a","c",NULL,1);
    test("(abd)|b|c|a","abd",NULL,1);
    test("(abd)|b|c|a","ab",NULL,1);
    fprintf(stderr,"\n\nAll Tests Passed\n");

    return 0;
}
