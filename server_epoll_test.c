#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>

#define MAXEVENTS 64

static int create_and_bind(char *port)
{
    struct addrinfo hints;
    struct addrinfo *result,*rp;
    int s,sfd;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    s = getaddrinfo(NULL,port,&hints,&result);
    if(s != 0){
        printf("getaddrinfo err\n");
        return -1;
    }
    for(rp = result;rp != NULL;rp = rp->ai_next){
        sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        printf("sfd:%d\n",sfd);
        if(sfd == -1){
            continue;
        }
        s = bind(sfd,rp->ai_addr,rp->ai_addrlen);
        if(s == 0){
            printf("bind ok\n");
            break;
        }
        printf("bind err\n");
        close(sfd);
    }
    if(rp == NULL){
        printf("could not bind\n");
    }
    freeaddrinfo(result);
    return sfd;
}

static int make_socket_non_blocking(int sfd)
{
    int flags,s;
    flags = fcntl(sfd,F_GETFL,0);
    if(flags == -1){
        printf("fcntl err\n");
        return -1;
    }
    flags |= O_NONBLOCK;
    s = fcntl(sfd,F_SETFL,flags);
    if(s == -1){
        printf("fcntl err2\n");
        return -1;
    }
    return 0;
}

int main(int argc,char *argv[])
{
    int sfd,s;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    if(argc != 2){
        printf("usage: %s [port]\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    sfd = create_and_bind(argv[1]);
    if(sfd == -1){
        printf("err line:%d\n",__LINE__);
        return -1;
    }
    s = make_socket_non_blocking(sfd);
    if(s == -1){
        printf("err line:%d\n",__LINE__);
        return -1;
    }
    s = listen(sfd,SOMAXCONN);
    if(s == -1){
        printf("err line:%d\n",__LINE__);
        return -1;
    }
    efd = epoll_create1(0);
    if(efd == -1){
        printf("err line:%d\n",__LINE__);
        return -1;
    }
    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&event);
    if(s == -1){
        printf("err line:%d\n",__LINE__);
        return -1;
    }
    events = calloc(MAXEVENTS,sizeof(event));
    while(1){
        int n,i;
        n = epoll_wait(efd,events,MAXEVENTS,-1);
        for(i = 0;i < n;i++){
            if(events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)){
                printf("err line:%d\n",__LINE__);
                close(events[i].data.fd);
                continue;
            }
            else if(sfd == events[i].data.fd){
                struct sockaddr in_addr;
                socklen_t in_len;
                int infd;
                char hbuf[NI_MAXHOST],sbuf[NI_MAXSERV];
                in_len = sizeof in_addr;
                infd = accept(sfd,&in_addr,&in_len);
                if(infd == -1){
                    if(errno == EAGAIN || errno == EWOULDBLOCK){
                        continue;
                    }
                    else{
                        printf("accept err\n");
                        continue;
                    }
                }
                s = getnameinfo(&in_addr,in_len,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);
                if(s == 0){
                    printf("accepted connection on descriptor %d (host=%s,port=%s)\n",infd,hbuf,sbuf); 
                }
                s = make_socket_non_blocking(infd);
                if(s == -1){
                    printf("err line:%d\n",__LINE__);
                    return -1;
                }
                event.data.fd = infd;
                event.events = EPOLLIN | EPOLLET;
                s = epoll_ctl(efd,EPOLL_CTL_ADD,infd,&event);
                if(s == -1){
                    printf("err line:%d\n",__LINE__);
                    return -1;
                }
            }
            else{
                int done = 0;
                ssize_t count;
                char buf[512];
                count = read(events[i].data.fd,buf,sizeof(buf));
                if(count == -1){
                    if(errno != EAGAIN){
                        printf("read err\n");
                        done = 1;
                    }
                    continue;
                }
                else if(count == 0){
                    done = 1;
                    continue;
                }
                s = write(1,buf,count);
                if(s == -1){
                    printf("write err\n");
                    return -1;
                }
                if(done){
                    printf("closed connection on descriptor %d\n",events[i].data.fd);
                    close(events[i].data.fd);
                }
            }
        }
    }
    free(events);
    close(sfd);
    return EXIT_SUCCESS;
}
