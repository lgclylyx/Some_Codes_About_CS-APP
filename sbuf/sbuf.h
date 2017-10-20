#ifndef sbuf_h
#define sbuf_h

#include <stdlib.h>
#include <semaphore.h>
typedef struct{
	int *buf;
	int n;
	int front;
	int rear;
	sem_t mutex;
	sem_t slots;
	sem_t items;
}sbuf_t;

#endif
