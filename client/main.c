#include <fcntl.h>
#include "cfunction.h"


int main(int argc, char* argv[]) {

    while (1) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        fprintf(stderr, "client cannot create socket");
        exit(2);
    }
    struct sockaddr_in addresstruct;
    int Port = strtol(argv[1], NULL, 0);
    addresstruct.sin_family = AF_INET;
    addresstruct.sin_port = htons(Port);

    const char *Host = "127.0.0.1";
    int res = inet_aton(Host, &addresstruct.sin_addr);
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


        send(sock_fd, "0", 1, 0);
        char buf[1024];
        int readed = 0;
        int i = 0;
        while ((readed += recv(sock_fd, buf, 1024, 0)) ==1024) {
            send(sock_fd, "0", 1, 0);
            fprintf(stderr, "%s %d\n", buf, readed);
            readed = 0;
            if (readed < -1) {
                if (shutdown(sock_fd, SHUT_RDWR)) {
                    fprintf(stderr, "cannot shut down");
                    return 2;
                }
                printf("naturalna śmierć");
            }

        }

    }
}
