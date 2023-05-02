#include<cstdio>
#include<iostream>
#include<stdlib.h>
#include"util.h"
using namespace std;


void errif(bool condition,const char * errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
