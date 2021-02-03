#ifndef SERWER_FUNCTION_H

#define SERWER_FUNCTION_H
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/poll.h>

void getOpt(int argc, char* argv[], char* o_arg, float *tempo);
in_addr_t isAddrOk(char* o_arg,  char* port, char* host);
void register_addr(int sockfd, char* host, in_port_t port);
int connection(int sockfd);
void fabryka(int pajpa, float tempo, char ASCI);
char* generate(char ASCI);
int set_non_blocking(int sockfd);


char* generate(char ASCI){
    char* buf=calloc(640, sizeof(char));
    memset(buf, ASCI, 640);
  //  printf("%s\n", buf);
    return buf;
}

void fabryka(int pajpa, float tempo, char ASCI)
{//blok 640 bajtów tempo * 2662
    float onsec=tempo*2662;
    struct timespec req={req.tv_sec=1, req.tv_nsec=0}, rem;
    int written=0;
    int check=0;

    struct timespec time={time.tv_nsec=0, time.tv_sec=0};
    while(1){
        if(ASCI>122) ASCI=65;
        if(ASCI>90 && ASCI<97) ASCI=97;
        char* buf=generate(ASCI);
        ASCI++;
        written+=check=write(pajpa, buf, 640);
        if(check!=640){
            if (errno == EAGAIN) {
                ASCI--;
            }
            else exit(1);
        }
        if(written>=(int)onsec-640)
        {
            nanosleep(&req, &rem);
            written=0;
        }
    }
}

int connection(int sockfd){
    struct sockaddr_in peer;
    socklen_t addr_len=sizeof(peer);
    int new_socket=accept(sockfd, (struct sockaddr *)&peer, &addr_len);
    if(new_socket==-1) {
        fprintf(stderr, "connection new socket -1");
        exit(2);
    }
    else{
        fprintf(stderr, "connnected to client %s port %d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
    }
    return new_socket;
}


void register_addr(int sockfd, char* host, in_port_t port){

    struct sockaddr_in inet_obj;
    inet_obj.sin_family = AF_INET;
    inet_obj.sin_port = htons(port);
    char* adr="127.0.0.1";
    if(strcmp("localhost", host)==0)
    {
        strcpy(host, adr);
        printf("%s", host);
    }

    int result = inet_aton(host,&inet_obj.sin_addr);
    if( ! result ) {
        fprintf(stderr,"uncorrect addres: %s\n",host);
        exit(1);
    }
    if( bind(sockfd,(struct sockaddr *)&inet_obj,sizeof(inet_obj)) ) {
        fprintf(stderr, "cannot bind port: %d host: %s result %d\n", port, host, result);
        exit(1);
    }

}

void getOpt(int argc, char* argv[], char* o_arg, float *tempo) {
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

int set_non_blocking(int sockfd)
{
    int flags, s;
    flags = fcntl(sockfd, F_GETFL, 0);
    if(flags == -1)
    {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    s = fcntl(sockfd, F_SETFL, flags);
    if(s == -1)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}


#endif //SERWER_FUNCTION_H
