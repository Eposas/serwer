#include "cfunction.h"


int main(int argc, char* argv[]) {

    char *o_arg = calloc(64, sizeof(char));
    float decompose=0;
    float tempo = 0;
    int storage=0;
    char *host = calloc(32, sizeof(char));
    char *port_temp = calloc(32, sizeof(char));
    struct timespec time_start;
    struct timespec time_stop;

    getOpt(argc, argv, &storage, &tempo, &decompose, o_arg);
    in_port_t port=isAddrOk(o_arg, port_temp, host);
    calcvalue(&storage, &tempo, &decompose);
    char* adr="127.0.0.1";
    if(strcmp("localhost", host)==0) {
        strcpy(host, adr);
        printf("%s", host);
    }

    printf("storage %d, tempo %f, decompose %f, host %s, port %d \n", storage, tempo, decompose, host, port);
    struct sigaction sack;
    sigemptyset(&sack.sa_mask);
    sack.sa_flags=0;
    sack.sa_handler=catcher;
    sigaction(SIGALRM, &sack, NULL);
    int owning=0;

        while (1) {
            clock_gettime(CLOCK_REALTIME, &time_start);
            if(owning >= storage-13*1024){
                fprintf(stderr, "babciu więcej już nie zmieszcze %d %d", owning, storage);
                exit(1);
            }
            printf("posiadam %d\n", owning);
            int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_fd == -1) {
                fprintf(stderr, "client cannot create socket");
                exit(2);
            }

            struct sockaddr_in addresstruct;
            addresstruct.sin_family = AF_INET;
            addresstruct.sin_port = htons(port);

            int res = inet_aton(host, &addresstruct.sin_addr);
            if (!res) {
                fprintf(stderr, "uncorret serwer addres");
                exit(2);
            }

            int proba = 11;
            while (--proba) {
                if (connect(sock_fd, (struct sockaddr *) &addresstruct, sizeof(addresstruct)) != -1) break;
            }
            if (!proba) {
                fprintf(stderr, "connection not accepted");
                exit(2);
            }
            fprintf(stderr, "nawiązane połączenie z serwerem %s (port %d)\n",
                    inet_ntoa(addresstruct.sin_addr), ntohs(addresstruct.sin_port));

            send(sock_fd, "request", 7, 0);
            char buf[1024];
            for (int i = 0; i < 13;) {
                if (recv(sock_fd, buf, 1024, 0) == 1024) {
                    i++;
                    owning+=1024;
                }
            }
            sleep_after_comsumpcion(tempo);
            clock_gettime(CLOCK_REALTIME, &time_stop);
            sleep_decompose(time_start, time_stop, decompose, &owning);
            if (close(sock_fd) == -1) {
                fprintf(stderr, "\nklopoty\n");
                exit(1);
            }
            char* message="dsa";
            int r=on_exit(exitfunction, (void*)message);
            if(r!=0){
                fprintf(stderr, "cannot set exit function");
                exit(1);
            }

        }
    }

