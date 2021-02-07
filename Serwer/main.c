#include "function.h"

int main(int argc, char *argv[]) {
    char *o_arg = calloc(64, sizeof(char));
    float tempo = 0;
    char *host = calloc(32, sizeof(char));
    char *port_temp = calloc(32, sizeof(char));
    struct pollfd descriptors[200];
    int end_server=0, flaga;
    int prev=0;
    getOpt(argc, argv, o_arg, &tempo);
    in_port_t port = isAddrOk(o_arg, port_temp, host);
    free(o_arg);

    int pipefd[2];
    if (pipe(pipefd) == -1 ) {
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
        int sock_fd, new_socket, struct_num=2;
        sock_fd=create_soc();
        register_addr(sock_fd, host, port);
        memset(descriptors, 0, sizeof(descriptors));
        descriptors[0].fd = sock_fd;
        descriptors[0].events = POLLIN;
        int timer_fd=create_and_set_timer();
        descriptors[1].fd = timer_fd;
        descriptors[1].events=POLLIN;
        int not_registered=0;
        do {
            add_clients(pipefd[0], &struct_num, &not_registered);
            if(poll(descriptors, struct_num, -1) < 0){
                fprintf(stdout, "poll error");
                break;
            }
            int current_size = struct_num;
            for (int i = 0; i < current_size; i++) {
                if (descriptors[i].revents == 0)
                    continue;
                if (descriptors[i].revents != POLLOUT && descriptors[i].fd!= timer_fd && descriptors[i].fd!= sock_fd) {
                    //sytuacja gdy klient zrobi ctrl + C
                    //printf("klient się rozlaczyl deskryptor: %d \n", descriptors[i].fd);
                    wasted(descriptors[i].fd, pipefd[0]);//marnuje i liczy zmarnowane bity
                    descriptors[i].fd = can_close(descriptors[i].fd, &flaga,
                                                  inet_ntoa(clients[descriptors[i].fd].sin_addr),
                                                  ntohs(clients[descriptors[i].fd].sin_port));
                    //zamyka deskryptor i tworzy raport
                }
                if (descriptors[i].fd == timer_fd) {
                    int a;
                    if (read(descriptors[i].fd, &a, 8) == -1) { //odczyt budzika
                        fprintf(stdout, "read error");
                    } else{ //raport co 5 sec
                        get_raport(pipefd[0],  current_size+not_registered-2, &prev);
                        //raport podaje ilość wszystkich klientów (bez fd timera i słuchającego) także tych co nie dostają bitów
                    }
                } else {
                    if (descriptors[i].fd == sock_fd) {
                        do {
                            struct sockaddr_in temp;
                            socklen_t addr_len = sizeof(temp);
                            new_socket = accept(sock_fd, (struct sockaddr *)&temp,&addr_len);
                            if (new_socket <= 0) {
                                if (errno != EWOULDBLOCK) {
                                    fprintf(stdout, "new sock error");
                                    end_server = 1;
                                }
                                break;
                            }
                            descriptors[struct_num+not_registered].fd = new_socket;
                            descriptors[struct_num+not_registered].events = POLLOUT;
                            memcpy(&clients[new_socket], &temp, addr_len);
                            //printf("%d %s %d \n",new_socket, inet_ntoa(clients[new_socket].sin_addr), ntohs(clients[new_socket].sin_port));
                            //printf do obserwacji
                            //licznik tych którzy nie trafili do polla
                            not_registered++;
                        } while (new_socket != -1);
                    } else {
                        if (descriptors[i].fd != -1) { //gdy klient nie zrobił ctrl+C, zapisuje i sprawdzam czy komplet
                            can_write(pipefd[0], descriptors[i].fd);
                            descriptors[i].fd = can_close(descriptors[i].fd, &flaga,
                                                          inet_ntoa(clients[descriptors[i].fd].sin_addr),
                                                          ntohs(clients[descriptors[i].fd].sin_port));
                        }
                    }
                    if (flaga) { //flaga która sprawdza czy jakiś fd został zamknięty
                        flaga = 0;
                        for (i = 0; i < struct_num; i++) {
                            if (descriptors[i].fd == -1) {
                                for (int j = i; j < struct_num; j++) {
                                    //użyte deskryptory nadpisuje kolejnymi, także nie dopuszczonymi do wysyłki
                                    //printf("zamieniam %d na %d \n", descriptors[j].fd, descriptors[j+1].fd);
                                    descriptors[j].fd = descriptors[j + 1].fd;
                                }
                                i--;
                                struct_num--;
                            }
                        }
                    }
                }
            }
        } while (end_server==0);
    }else {
        //rodzic
        close(pipefd[0]);
        fabryka(pipefd[1], tempo, 'A');
        //fabryka bitów
    }

    return 0;
}
