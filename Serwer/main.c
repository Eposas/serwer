#include <errno.h>
#include "function.h"

int main(int argc, char *argv[]) {

    char *o_arg = calloc(64, sizeof(char));
    float tempo = 0;
    char *host = calloc(32, sizeof(char));
    char *port_temp = calloc(32, sizeof(char));

    getOpt(argc, argv, o_arg, &tempo);
    in_port_t port = isAddrOk(o_arg, port_temp, host);
    printf("host: %s port: %s port %d \n", host, port_temp, port);
    free(o_arg);
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe failure");
        exit(2);
    }
    int c_pid = fork();
    if (c_pid == -1) {
        fprintf(stderr, "fork failure");
        exit(2);
    }
    if (c_pid == 0) {
        close(pipefd[1]); //close unused

        struct epoll_event ev, events[3];
        int sock_fd, new_socket, nfds, epollfd, n;

        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == -1) {
            fprintf(stderr, "cannot create socket");
            exit(1);
        }
        register_addr(sock_fd, host, port);
        if (listen(sock_fd, 10)) {
            fprintf(stderr, "cannot change mode to pasive");
            exit(1);
        }

        epollfd = epoll_create(3);
        if (epollfd == -1) {
            fprintf(stderr, "epoll create error");
            exit(1);
        }
        ev.data.fd = sock_fd;
        ev.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_fd, &ev) == -1) {
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }
        for (;;) {
            nfds = epoll_wait(epollfd, events, 3, -1);
            if (nfds == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
            for (n = 0; n < nfds; ++n) {
                if (events[n].data.fd == sock_fd) {
                    new_socket = connection(sock_fd);
                    if(new_socket==-1){
                        if((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
                        else{
                            perror("accept");
                            break;
                        }
                    }
                    set_non_blocking(new_socket);
                    ev.events = EPOLLIN|EPOLLET;
                    ev.data.fd = new_socket;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
                        fprintf(stderr, "epoll ctl error");
                        exit(1);
                    }
                } else {
                    int val;
                    char buf[640]={0};
                    int readed=read(pipefd[0], buf, 640);
                    if((val=send(events[n].data.fd, buf, 640, 0)) == -1)
                    {
                        perror("send");
                        break;
                    }
                    fprintf(stderr, "%s %d %d \n", buf,val, events[n].data.fd);
                    epoll_ctl(nfds, EPOLL_CTL_DEL, events[n].data.fd, &ev);
                    //close(events[n].data.fd);
                }
            }
        }
    } else {
        //rodzic
        close(pipefd[0]); //close unused
        char c = fabryka(pipefd[1], tempo, 'A');
        printf("%c", c);
        //fabryka bitów
    }

    return 0;
}
