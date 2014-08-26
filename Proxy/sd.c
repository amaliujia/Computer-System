#include "sd.h"
#include "cache.h"
#include "wrapper.h"

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *accept_hdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding_hdr = "Accept-Encoding: gzip, deflate\r\n";
static const char *connection = "Connection: close\r\n";
static const char *proxy = "Proxy-Connection: close\r\n";

void afterAccept(int fd){
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char host[MAXLINE], path[MAXLINE];
	char transSend[MAXLINE], transResponse[MAXLINE];
	char *objectCache;
	char headers[MAXLINE], hostHeader[MAXLINE];
	int port;
	rio_t rio;
	
	//initialize
	objectCache = malloc(MAX_OBJECT_SIZE);
	if(objectCache <= 0){
		return;
	}
	memset(buf, 0, MAXLINE);
	memset(method, 0, MAXLINE);
	memset(url, 0, MAXLINE);
	memset(version, 0, MAXLINE);					
	memset(host, 0, MAXLINE);
	memset(path, 0, MAXLINE);
	memset(transSend, 0, MAXLINE);		
	memset(transResponse, 0, MAXLINE);
	memset(objectCache, 0, MAX_OBJECT_SIZE);
	memset(headers, 0, MAXLINE);
	memset(hostHeader, 0, MAXLINE);

	//proxy read http request from client socket	
    Rio_readinitb(&rio, fd);
    Rio_readlineb_s(&rio, buf, MAXLINE);
	sscanf(buf, "%s %s %s", method, url, version);
	if(strcasecmp(method, "GET")){
		clienterror(fd, method, "501", "Not Implemented", "Not surpport POST method yet");
		free(objectCache);
		return; 
    }
	read_requesthdrs_s(&rio, headers, hostHeader);

	if(parse_url(url, host, path, &port)){
		clienterror(fd, "HTTP", "404", "Not Found", "Network portocal error");
		free(objectCache);
		return;
	}

	//check if cache already
	if(!cacheGet(url, objectCache, &cache)){
		//cache hit
		Rio_writen_s(fd, objectCache, strlen(objectCache)); 		
		free(objectCache);
		return;	
	}	
	//if cache miss, execute next part
	
	//build http headers
	sprintf(transSend, "GET  ");
	sprintf(transSend, "%s%s  ", transSend, path);
	sprintf(transSend, "%sHTTP/1.0\r\n", transSend);
	if(strlen(hostHeader) != 0){
		sprintf(transSend, "%s%s", transSend, hostHeader);
	}else{
		sprintf(transSend, "%s%s%s\r\n", transSend, "Host: ", host);			
	}
	sprintf(transSend, "%s%s", transSend, user_agent_hdr);
	sprintf(transSend, "%s%s", transSend, accept_hdr);
	sprintf(transSend, "%s%s", transSend, accept_encoding_hdr);
	sprintf(transSend, "%s%s", transSend, connection);
	sprintf(transSend, "%s%s", transSend, proxy);
	if(strlen(headers) != 0)
		sprintf(transSend, "%s%s", transSend, headers);
	sprintf(transSend, "%s\r\n", transSend);
	
	//connect remote server	
	int transfd, contentSize = 0;
	ssize_t size = 0;
	rio_t transrio;
	transfd = Open_clientfd_r(host, port);
	if(transfd < 0){
		clienterror(fd, "HTTP", "400", "request error", "please check your request, especially the correctness of hostname and port");
		free(objectCache);
		return;		
	}
    Rio_readinitb(&transrio, transfd); 
	Rio_writen_s(transfd, transSend, strlen(transSend));
	while((size = Rio_readlineb_s(&transrio, transResponse, MAXLINE)) > 2){
		strcat(objectCache, transResponse);
		contentSize += size;
		Rio_writen_s(fd, transResponse, strlen(transResponse)); 
	}
		Rio_writen(fd, "\r\n", 2);
		strcat(objectCache, "\r\n");
		contentSize += 2;
		while((size = Rio_readnb_s(&transrio, transResponse, MAXLINE)) != 0){
			contentSize += size;
			if(contentSize <= MAX_OBJECT_SIZE)
				strcat(objectCache, transResponse);
			Rio_writen_s(fd, transResponse, size);	
		}
	
	if(contentSize <= MAX_OBJECT_SIZE){
		cacheSet(url, objectCache, &cache);
	}
	free(objectCache);	
	Close(transfd);	
		
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg){
	char buf[MAXLINE], body[MAXLINE];
	
	sprintf(body, "<html><title>Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s %s: %s\r\n", body, cause, errnum, shortmsg);
	sprintf(body, "%s<p>%s\r\n", body, longmsg);
	sprintf(body, "%s<hr><em>Proxy</em>\r\n", body);

	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen_s(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen_s(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen_s(fd, buf, strlen(buf));
	Rio_writen_s(fd, body, strlen(body));	
	return;
}

void read_requesthdrs(rio_t *rp){
	char buf[MAXLINE];
	Rio_readlineb_s(rp, buf, MAXLINE);
	while(strcmp(buf, "\r\n")){
		Rio_readlineb_s(rp, buf, MAXLINE);
	}
	return;
}

//get client's http request headers
void read_requesthdrs_s(rio_t *rp, char *headers, char *hostHeader){
    char buf[MAXLINE];
    Rio_readlineb_s(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){
        Rio_readlineb_s(rp, buf, MAXLINE);
    	if(strcmp(buf, "Host:") == 0)
			sprintf(headers, "%s%s", hostHeader, buf);
		else if((strncmp(buf, "User-Agent:", 11) != 0) && (strncmp(buf, "Accept:", 7) != 0) && (strncmp(buf, "Accept-Encoding:", 16) != 0) && (strncmp(buf, "Connection:", 11) != 0)  && (strncmp(buf, "Proxy-Connection:", 17) != 0)){
			sprintf(headers, "%s%s", headers, buf);
	}
	 }
	return;
}

 
int parse_url(char *url, char *hostname, char *pathname, int *port){
	int defaultport = 80;
	int len = strlen(url);	
	if(strncmp(url, "http://", 7) != 0){
		printf("wrong network protocal\n");
		return 1;
	}
	char *hosthead = url + 7; 
	char *pathhead = strchr(hosthead, '/');
	int pathLen, hostLen;
	if(pathhead == NULL){
		strncpy(pathname, "/", 1);
		hostLen = len - 7;
	}else{
		pathLen = len - (pathhead - url);
		strncpy(pathname, pathhead, pathLen);
		pathname[pathLen] = '\0';
		hostLen =  pathhead - hosthead;
	}
	strncpy(hostname, hosthead, hostLen);
	hostname[hostLen] = '\0';	
	char *porthead = strchr(hostname, ':');
	if(porthead != NULL){
			defaultport = atoi(porthead + 1);
			hostLen = porthead - hostname;
			memset(hostname, 0, strlen(hostname));
    		strncpy(hostname, hosthead, hostLen);
			hostname[hostLen] = '\0';
	}
	*port = defaultport;
	return 0;	
}
