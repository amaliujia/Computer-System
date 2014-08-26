#ifndef _SD_H_
#define _SD_H_

#include "include.h"

void afterAccept(int fd);
void clienterror(int fd, char *cause, char *errnum,
				char *shortmsg, char *longmsg);
void read_requesthdrs(rio_t *rp);
void read_requesthdrs_s(rio_t *rp, char *headers, char *hostHeader);
int parse_url(char *url, char *host, char *path, int *port);
void proxy_request();
#endif
