#ifndef MYQUEUE_H_
#define MYQUEUE_H_

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <semaphore.h>


typedef void* (*job_function)(void*);
typedef void* job_arg;
typedef sem_t job_id;

typedef struct job {
	job_function func;
	job_arg arg;
	job_id id;
} job_t;


struct myqueue_entry {
	job_t job;
	STAILQ_ENTRY(myqueue_entry) entries;
};

STAILQ_HEAD(myqueue_head, myqueue_entry);

typedef struct myqueue_head myqueue;

static void myqueue_init(myqueue* q) {
	STAILQ_INIT(q);
}

static bool myqueue_is_empty(myqueue* q) {
	return STAILQ_EMPTY(q);
}


static job_id myqueue_push(myqueue* q, job_function start_routine, job_arg arg) {
	struct myqueue_entry* entry = malloc(sizeof(struct myqueue_entry));
	entry->job.func = start_routine;
    entry->job.arg = arg;
	sem_init(&entry->job.id, 0, 1);
	STAILQ_INSERT_TAIL(q, entry, entries);
	return entry->job.id;
}

static job_t myqueue_pop(myqueue* q) {
	assert(!myqueue_is_empty(q));
	struct myqueue_entry* entry = STAILQ_FIRST(q);
	STAILQ_REMOVE_HEAD(q, entries);
	job_t job = entry->job;
	free(entry);
	return job;
}

#endif
