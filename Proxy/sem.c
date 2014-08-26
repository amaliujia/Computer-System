#include "sem.h"

void semInit(sem_t *rlock, sem_t *wlock){
	Sem_init(rlock, 0, 1);
	Sem_init(wlock, 0 , 1);	
	readerCount = 0;
}

void semReadP(sem_t *rlock, sem_t *wlock){
	P(rlock);
	readerCount++;
	if(readerCount == 1){
		P(wlock);	
	}
	V(rlock);
}

void semReadV(sem_t *rlock, sem_t *wlock){
    P(rlock);
    readerCount--;
    if(readerCount == 0){
        V(wlock);
    }
    V(rlock);
}
