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
#include <signal.h>




void getOpt(int argc, char* argv[], int* storage, float* tempo, float*  decompose, char* o_arg);
in_addr_t isAddrOk(char* o_arg,  char* port, char* host);
void calcvalue(int* storage, float* tempo, float* decompose);
void sleep_after_comsumpcion(float tempo);
void sleep_decompose(struct timespec start, struct timespec stop, float decompose, int* mam);
void catcher(int sig);
void exitfunction(int status, void* message);
double calc_time(struct timespec t1, struct timespec t2);


void exitfunction(int status, void* message){
    printf("%s", (char*)message);

}


double calc_time(struct timespec t1, struct timespec t2)
{
   double val= t1.tv_sec - t1.tv_sec + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
   return val;
}

void sleep_decompose(struct timespec start, struct timespec stop, float decompose, int* mam){

    double value=stop.tv_sec - start.tv_sec + (stop.tv_nsec - start.tv_nsec) / 1000000000.0;
    int del=(int)(value*decompose);
    *mam=*mam-del;
    if(*mam<0) *mam=0;
    printf("czas petli %f niszcze %d\n", value, del);
    alarm((int)value);
}

void catcher(int sig){
    printf("signal cought %d \n", sig);
}

void sleep_after_comsumpcion(float tempo) {
    struct timespec sleeptime;
    struct timespec wake;
    long int tmp = 1024 / (tempo * 4435) * 1000000000;
    sleeptime.tv_nsec = tmp % 1000000000;
    sleeptime.tv_sec = tmp - (tmp % 1000000000);
    printf("spie %ld \n", tmp);
    nanosleep(&sleeptime, &wake);
}
void calcvalue(int* storage, float* tempo, float* decompose){

    *storage=*storage*30*1024;
    *tempo=*tempo*4435;
    *decompose=*decompose*819;
}


in_addr_t isAddrOk(char* o_arg,  char* port, char* host)
{
    sscanf(o_arg, "%[^:]:%s", host, port);
    if(strtol(port, NULL, 0) == 0)
    {
        strcpy(port, host);
        strcpy(host, "localhost");
    }
    return strtol(port, NULL, 0);
}

void getOpt(int argc, char* argv[], int* storage, float* tempo, float*  decompose, char* o_arg){
    if((argc<7))
    {
        fprintf(stderr, "Not enough args %s\n", argv[0]);
        exit(2);
    }
    char* endptr;
    int opt;
    while ((opt = getopt(argc, argv, "c:p:d:")) != -1) {
        switch (opt) {
            case 'c':
                *storage = strtol(optarg, NULL, 0);
                if(*storage<=0)
                {
                    fprintf(stderr, "value <=0 is not acceptable");
                    exit(2);
                }
                break;
            case 'p':
                *tempo= strtof(optarg, &endptr);
                if(*tempo<=0){
                    fprintf(stderr, "value <=0 is not acceptable");
                    exit(2);
                }
                break;
            case 'd':
                *decompose=strtof(optarg, &endptr);
                if(*decompose<=0){
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