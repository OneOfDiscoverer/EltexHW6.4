#include "main.h"
#define NUMOFTREAD 10
#define MAX_EVENTS 100

/*
    обработка с помощью select poll epoll
*/

sem_t lock;

void* receiver(void* argc){
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS]; 
    int nr_events, i, epfd;
    epfd = epoll_create1(0);
    while(1){
        list* tmp;
        while(1){
            sem_wait(&lock);
            if(tmp = getAt(0)) {
                event.data.fd = tmp->bk.sockdes;
                remove_at(0);
                sem_post(&lock);
                event.events = EPOLLIN;
                if(epoll_ctl (epfd, EPOLL_CTL_ADD, event.data.fd, &event)){
                    perror ("epoll_ctl");
                }
                break;
            }   
            sem_post(&lock);
            nr_events = epoll_wait(epfd, events, MAX_EVENTS, 0);
            if (nr_events < 0) {
                perror ("epoll_wait");
            }
            for(i = 0; i < nr_events; i++){
                char buffer[1024] = {0};
                ssize_t sz = read(events[i].data.fd, buffer, 1024);
                if(sz < 0){
                    perror("read");
                }
                else if(sz == 0){ //костыль закрытия сокета
                    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);
                    //perror("read");
                    continue;
                }
                strcat(buffer, "+ ");
                if(send(events[i].data.fd, buffer, strlen(buffer), 0) < 0){
                    perror("send");
                }
            }
        }
    }    
}

int main(int argc, char* argv[]){
    pthread_t thrd[NUMOFTREAD];
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket not opened");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.0.136");
    address.sin_port = htons(8090);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    sem_init(&lock, 0, 1);

    for(int i = 0; i < NUMOFTREAD; i++){
        pthread_create(&thrd[i], NULL, receiver, NULL);
    }

    if(listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1){
        book bk = {0};
        if((bk.sockdes = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pushBack(&bk);
    }    
    return 0;
}
