#ifndef WRAPPERS_NETWORK_H
#define WRAPPERS_NETWORK_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "logging.h"

extern int tcp_socket_ip4(void);
extern int wrap_socket(int domain, int type, int protocol);
extern int wrap_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern int wrap_listen(int sockfd, int backlog);
extern int accept_without_addr(int sockfd);
extern int wrap_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern int wrap_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern ssize_t wrap_recv(int sockfd, void *buf, size_t len, int flags);
extern ssize_t wrap_send(int sockfd, const void *buf, size_t len, int flags);

extern const char* easy_ntop_ip4(const void *src);
extern const char* wrap_ntop(int af, const void *src, char *dst, socklen_t size);
extern int easy_pton_ip4(const char *src, void* dst);
extern int wrap_pton(int af, const char *src, void *dst);


extern struct sockaddr_in* get_sockaddr_in(const char *ip, uint16_t port);

#endif
