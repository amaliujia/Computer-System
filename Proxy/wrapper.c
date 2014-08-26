#include "wrapper.h"

ssize_t Rio_readn_s(int fd, void *ptr, size_t nbytes){
    ssize_t n;
    if ((n = rio_readn(fd, ptr, nbytes)) < 0){
			printf("Rio_readn error\n");
			n = 0;
		}
    return n;
}

ssize_t Rio_readnb_s(rio_t *rp, void *usrbuf, size_t n){
    ssize_t rc;
    if ((rc = rio_readnb(rp, usrbuf, n)) < 0){
		printf("Rio_readnb error\n");
		rc = 0;
	}
    return rc;	
}

ssize_t Rio_readlineb_s(rio_t *rp, void *usrbuf, size_t maxlen){
    ssize_t rc;
    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0){
		printf("Rio_readlineb error\n");
		rc = 0;
	}
    return rc;
}

void Rio_writen_s(int fd, void *usrbuf, size_t n){
	if (rio_writen(fd, usrbuf, n) != n)
		printf("Rio_writen error\n");
	
}
