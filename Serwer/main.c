
#include <sys/ioctl.h>
#include "function.h"

int main(int argc, char *argv[]) {

    char *o_arg = calloc(64, sizeof(char));
    float tempo = 0;
    char *host = calloc(32, sizeof(char));
    char *port_temp = calloc(32, sizeof(char));
    int rc, on=1, len, flaga;
    int end_connection;
    struct pollfd descriptors[200];
    int end_server=0;
    int desc[200]={0};
    int permits[200]={0};

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
        int sock_fd, new_socket, struct_num=1;

        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == -1) {
            fprintf(stderr, "cannot create socket");
            exit(1);
        }

        rc = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                        (char *) &on, sizeof(on));
        if (rc < 0) {
            perror("setsockopt() failed");
            close(sock_fd);
            exit(-1);
        }

        rc = ioctl(sock_fd, FIONBIO, (char *) &on);
        if (rc < 0) {
            perror("ioctl() failed");
            close(sock_fd);
            exit(-1);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family      = AF_INET;
        int result = inet_aton(host,&addr.sin_addr);
        if( ! result ) {
            fprintf(stderr,"uncorrect addres: %s\n",host);
            exit(1);
        }
        addr.sin_port= htons(port);
        rc = bind(sock_fd,
                  (struct sockaddr *)&addr, sizeof(addr));
        if (rc < 0)
        {
            perror("bind() failed");
            close(sock_fd);
            exit(-1);
        }

        rc = listen(sock_fd, 32);
        if (rc < 0) {
            perror("listen() failed");
            close(sock_fd);
            exit(-1);
        }
        memset(descriptors, 0, sizeof(descriptors));
        descriptors[0].fd = sock_fd;
        descriptors[0].events = POLLIN;

        do {

            if(poll(descriptors, struct_num, -1) < 0){
                fprintf(stdout, "poll error");
                break;
            }
            int current_size = struct_num;
            for (int i = 0; i < current_size; i++) {
                if (descriptors[i].revents == 0)
                    continue;
                if (descriptors[i].revents != POLLIN) {
                    fprintf(stdout, "revent error");
                    end_server = 1;
                    break;
                }
                if (descriptors[i].fd == sock_fd) {
                    do {
                        new_socket = accept(sock_fd, NULL, NULL);
                        if (new_socket < 0) {
                            if (errno != EWOULDBLOCK) {
                                fprintf(stdout, "new sock error");
                                end_server = 1;
                            }
                            break;
                        }
                        descriptors[struct_num].fd = new_socket;
                        descriptors[struct_num].events = POLLIN;
                        struct_num++;
                    } while (new_socket != -1);
                } else {

                        int zajetosc;
                            if (ioctl(pipefd[0], FIONREAD, &zajetosc) != -1) {
                                if (zajetosc > 1024 * 13) {
                                    permits[descriptors[i].fd] = 1;
                                    printf("deskryptor %d uzyskał pozwolenie \n", descriptors[i].fd);
                                } else {
                                    permits[descriptors[i].fd] = 0;
                                }
                            }

                        if(permits[descriptors[i].fd]==1){
                            int val;
                            char buf[1024];
                            read(pipefd[0], buf, 1024);
                            if ((val = send(descriptors[i].fd, buf, 1024, 0)) == -1) {
                                perror("send");
                                break;
                            }
                            desc[descriptors[i].fd]++;
                            fprintf(stderr, "%d %d %d \n", val, desc[descriptors[i].fd], descriptors[i].fd);
                        }

                    if(desc[descriptors[i].fd]==13){
                        desc[descriptors[i].fd]=0;
                        close(descriptors[i].fd);
                        descriptors[i].fd = -1;
                        flaga = 1;
                        printf("podejrzane %d \n", desc[descriptors[i].fd]);
                        permits[descriptors[i].fd]=0;
                    }
                }
                if (flaga) {
                    flaga = 0;
                    for (i = 0; i < struct_num; i++) {
                        if (descriptors[i].fd == -1) {
                            for (int j = i; j < struct_num; j++) {
                                descriptors[j].fd = descriptors[j + 1].fd;
                            }
                            i--;
                            struct_num--;
                            printf("tutaj");
                        }
                    }
                }
            }
        } while (end_server == 0);
    }else {
        //rodzic
        if (fcntl(pipefd[1], F_SETFL, O_NONBLOCK) < 0) exit(2);
        close(pipefd[0]);
        fabryka(pipefd[1], tempo, 'A');
        //fabryka bitów
    }

    return 0;
}

/*
int val;
char buf[1024]={0};
int zajetosc=0;
while(ioctl(pipefd[0], FIONREAD, &zajetosc)!=-1){
if(zajetosc>1024*13) break;
}
for(int i=0; i<13; i++) {
int readed=0;
while (readed < 1024) {
readed += read(pipefd[0], buf, 1024);
}
if ((val = send(events[n].data.fd, buf, 1024, 0)) == -1) {
perror("send");
break;
}
fprintf(stderr, "%s %d %d \n", buf, val, events[n].data.fd);
}*/


/* rc = recv(descriptors[i].fd, buffor, sizeof(buffor), 0);
  if (rc < 0) {
      if (errno != EWOULDBLOCK) {

          end_connection = 1;
      }
      break;
  }
  if (rc == 0) {
      fprintf( stdout, "Connection closed\n");
      end_connection = 1;
      break;
  }*/