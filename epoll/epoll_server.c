#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>

#define PORT 8081
#define BACKLOG 1024
#define MAX_EVENTS 12
#define DEFAULT_TIMEOUT 100 /* millisecond */
#define BUF_SIZE 1024

int process(int conn, char *buf) {
    bzero(buf, sizeof(buf));
    read(conn, buf, sizeof(buf));
    if(!strncmp(buf, "exit\n", sizeof(buf))) {
        return -1;
    }
    printf("%s", buf);
    write(conn, buf, strlen(buf));
    return 0;
}

int main() {
    
    int fd;
    int epfd;
    struct sockaddr_in server;
    struct epoll_event *events;
    struct epoll_event ee;
    int stop = 0;
    int conn;
    int res;

    char buf[BUF_SIZE];

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Fail to create socket");
        exit(1);
    }

    printf("Create Socket\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if(bind(fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Fail to bind socket");
        exit(1);
    }

    printf("Bind Socket\n");

    if(listen(fd, BACKLOG) == -1) {
        perror("Fail to listen socket");
        exit(1);
    }

    printf("Listen Socket\n");

    epfd = epoll_create(1024);
    if(epfd == -1) {
        perror("epoll_create");
        exit(1);
    }

    ee.data.fd = fd;
    ee.events = 0;
    ee.events |= EPOLLIN;
    ee.events |= EPOLLOUT;
    
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ee) == -1) {
        perror("epoll_ctl");
        exit(1);
    }

    events = calloc(MAX_EVENTS, sizeof(events));

    while(!stop) {
        int j;
        int nfds;
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if(nfds == -1) {
            perror("epoll_wait");
            break;
        }
        for(j = 0; j < nfds; j++) {
            struct epoll_event *e = events + j;

            if(e->data.fd == fd) {
                
                struct sockaddr_storage client;
                socklen_t client_len;
                client_len = sizeof(client);

                conn = accept4(fd, &client, (struct sockaddr*)&client_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
                if(conn == -1) {
                    perror("Accept socket error\n");
                    break;
                }

                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn;
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &ev) == -1) {
                    perror("conn");
                    break;
                }
            } else if(e->events & EPOLLIN) {
                res = process(e->data.fd, buf);
                if(res == -1) {
                    sprintf(buf, "bye\n");
                    write(conn, buf, strlen(buf));
                    epoll_ctl(epfd, EPOLL_CTL_DEL, e->data.fd, NULL);
                    close(e->data.fd);
                }

            } else if(e->events & (EPOLLRDHUP | EPOLLHUP)) {
                epoll_ctl(epfd, EPOLL_CTL_DEL, e->data.fd, NULL);
                close(e->data.fd);
            }
        }
    }

    free(events);
    close(epfd);
    close(fd);

}