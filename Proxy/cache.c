#include "cache.h"
#include "sem.h"


static int linenum = MAX_CACHE_SIZE / MAX_OBJECT_SIZE;

// init my cache
void cacheInit(proxyCache *cache){
	int i;
	for(i = 0; i < linenum; i++){
		cache->line[i].valid = 0;
		cache->line[i].block = malloc(MAX_OBJECT_SIZE);
		cache->line[i].tag = malloc(MAXLINE);	
		cache->line[i].counter = 0;
	} 
	semInit(&rlock, &wlock);	
}

// return 0 show success, 1 indicate failure
int cacheGet(char *target, char *response, proxyCache *cache){
	int i;
	semReadP(&rlock, &wlock);	
	for(i = 0; i < linenum; i++){
		if(cache->line[i].valid == 1 && strcmp(cache->line[i].tag, target) == 0){
			strcpy(response, cache->line[i].block);
			cache->line[i].counter = linenum;
			semReadV(&rlock, &wlock);
			cacheAdjust(cache);
			return 0;
		}	
	}
    semReadV(&rlock, &wlock);
	return 1;	
}

// insert record in to cache, if cache is full, 
// use LRU to evict
int cacheSet(char *tag, char *text, proxyCache *cache){
	if(strlen(text) > MAX_OBJECT_SIZE){
		return 1;
	}
	//find a free line	
	int i;
	P(&wlock);
	for(i = 0; i < linenum; i++){
		if(cache->line[i].valid == 0){
			cache->line[i].valid = 1;
			cache->line[i].counter = linenum + 1;
			strcpy(cache->line[i].tag, tag);
			strcpy(cache->line[i].block, text);
			V(&wlock);	
			cacheAdjust(cache);
			return 0;
		}
	}
	V(&wlock);
	//LRU
	semReadP(&rlock, &wlock);
	int x = cache->line[0].counter;
	int index = 0;	
	for(i = 1; i < linenum; i++){
		if(x > cache->line[i].counter){
			x =	cache->line[i].counter;
			index = i; 
		}						
	}
	semReadV(&rlock, &wlock);
	P(&wlock);
	cache->line[index].counter = linenum + 1;
	strcpy(cache->line[index].tag, tag);
	strcpy(cache->line[index].block, text);
	V(&wlock);
	cacheAdjust(cache);
	return 0;
}

// adjust counter
void cacheAdjust(proxyCache *cache){
	int i;
	P(&wlock);
	for(i = 0; i < linenum; i++){
		cache->line[i].counter--;	
	}
	V(&wlock);
}

// free pointers allocated by malloc 
void cacheDestroy(proxyCache *cache){
    int i;
    for(i = 0; i < linenum; i++){
        free(cache->line[i].block);
        free(cache->line[i].tag);
    }
}

