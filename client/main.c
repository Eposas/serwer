#include "cfunction.h"


int main(int argc, char* argv[]) {

    char *o_arg = calloc(64, sizeof(char));
    float tempo = 0;
    int storage=0;
    char *host = calloc(32, sizeof(char));
    char *port_temp = calloc(32, sizeof(char));

    getOpt(argc, argv, &storage, &tempo, o_arg);
    in_port_t port=isAddrOk(o_arg, port_temp, host);
    calcvalue(&storage, &tempo);
    char* adr="127.0.0.1";
    if(strcmp("localhost", host)==0) {
        strcpy(host, adr);
        printf("%s", host);
    }

    printf("storage %d, tempo %f, decompose %f, host %s, port %d \n", storage, tempo, decompose, host, port);
    //set_alarm();
    struct timespec start;

    clock_gettime(CLOCK_REALTIME, &start);

        while (1) {
            struct timespec  stop, time_start/*, time_stop, time_rel*/;
            int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_fd == -1) {
                fprintf(stderr, "client cannot create socket");
                exit(2);
            }

            printf("posiadam %d\n", owning);

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

           clock_gettime(CLOCK_MONOTONIC, &time_start);

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
              //  if(i==0)  clock_gettime(CLOCK_MONOTONIC, &time_rel);
            }

            clock_gettime(CLOCK_REALTIME, &stop);
            sleep_after_comsumpcion(tempo, calc_time(stop, start));
            start.tv_nsec=stop.tv_nsec;
            start.tv_sec=stop.tv_sec;

          //  clock_gettime(CLOCK_MONOTONIC, &time_stop);
           /* double a=calc_time(&time_rel ,&time_start);
            double b=calc_time(&time_stop ,&time_rel );*/

          /*  char eachblock[100];
            sprintf(eachblock, "connection %f \n", calc_time(time_rel, time_start));
            int r=on_exit(exitfunction, (void*)eachblock);
            if(r!=0){
                fprintf(stderr, "cannot set exit function");
                exit(1);
            }*/
            if(owning >= storage-13*1024){
                close(sock_fd);
                raport(adr, port);
                exit(0);
            }

            if (close(sock_fd) == -1) {
                fprintf(stderr, "\nklopoty\n");
                exit(1);
            }



        }
    }

