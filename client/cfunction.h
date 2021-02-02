#ifndef CLIENT_CFUNCTION_H
#define CLIENT_CFUNCTION_H
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>




void getOpt(int argc, char* argv[], int* storage, float* tempo, float*  decompose, char* host);
void getOpt(int argc, char* argv[], int* storage, float* tempo, float*  decompose, char* o_arg){
    if(argc<2 || argc>4)
    {
        fprintf(stderr, "Not enough args %s\n", argv[0]);
        exit(2);
    }
    char* endptr;
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                *tempo = strtof(optarg, &endptr);
                if(*tempo<=0)
                {
                    fprintf(stderr, "value <=0 is not acceptable");
                    exit(2);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-f float]\n", argv[0]);
                exit(2);
        }
    }
    if(optind<argc)
    {
        strcpy(o_arg,argv[optind]);
    }
    else{
        fprintf(stderr, "Previous security doesnt work");
        exit(2);
    }
}

#endif