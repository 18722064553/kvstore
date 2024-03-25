#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include "kvstore.h"

int accept_cb(int fd);
int recv_cb(int fd);
int send_cb(int fd);

int epfd = 0;

struct conn_item connlist[1048576] = {0};

int init_server(unsigned short port){
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	// memset用于将地址为serveraddr的大小为sizeof的空间初始化为0
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	// bind用于将一个套接字与一个本地地址关联起来，以便于接收来自该地址的数据或者向该地址发送数据。
	if(-1 == bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))){
		perror("bind");
		return -1;
	}
	listen(sockfd, 10);
	return sockfd;
}

void set_event(int fd, int event, int flag){
	struct epoll_event ev;
	ev.events = event;
	ev.data.fd = fd;

	// flag = 1 add, flag = 0 mod
	if(flag){epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);}
	else{epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);}
}


int accept_cb(int fd){
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int clientfd = accept(fd, (struct sockaddr*)&clientaddr, &len);
	if(clientfd < 0) return -1;
	set_event(clientfd, EPOLLIN, 1);

	connlist[clientfd].fd = clientfd;
	memset(connlist[clientfd].rbuffer, 0, BUFFER_LENGTH);
	connlist[clientfd].rlen = 0;
	memset(connlist[clientfd].wbuffer, 0, BUFFER_LENGTH);
	connlist[clientfd].wlen = 0;

	connlist[clientfd].recv_t.recv_callback = recv_cb;
	connlist[clientfd].send_callback = send_cb;
	
	return clientfd;
}

int recv_cb(int fd){
	char *buffer = connlist[fd].rbuffer;
	int idx = connlist[fd].rlen;

	int count = recv(fd, buffer, BUFFER_LENGTH, 0);
	if(count == 0){
		printf("disconnect\n");
		epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		return -1;
	}
	connlist[fd].rlen = count;

	kvstore_request(&connlist[fd]);
	connlist[fd].wlen = strlen(connlist[fd].wbuffer);

	set_event(fd, EPOLLOUT, 0);
	return count;
}

int send_cb(int fd){
	char *buffer = connlist[fd].wbuffer;
	int idx = connlist[fd].wlen;
	int count = send(fd, buffer, idx, 0);
	set_event(fd, EPOLLIN, 0);
	return count;
}

void epoll_entry(void){
	int port_count = 20;
	unsigned short port = 4096;
	int i = 0;

	epfd = epoll_create(1);

	for(int i = 0; i < port_count; ++i){
		int sockfd = init_server(port + i);
		connlist[sockfd].fd = sockfd;
		connlist[sockfd].recv_t.accept_callback = accept_cb;
		set_event(sockfd, EPOLLIN, 1);
	}

	struct epoll_event events[1024] = {0};

	while(1){  // mainloop();
		int nready = epoll_wait(epfd, events, 1024, -1);
		int i = 0;
		for(i = 0; i < nready; ++i){
			int connfd = events[i].data.fd;
			if(events[i].events & EPOLLIN){
				int count = connlist[connfd].recv_t.recv_callback(connfd);
				printf("recv count: %d <-- buffer: %s\n", count, connlist[connfd].rbuffer);
			}
			else if(events[i].events & EPOLLOUT){
				int count = connlist[connfd].send_callback(connfd);
				printf("send --> buffer: %s\n",  connlist[connfd].wbuffer);
			}
		}
	}
	
}



