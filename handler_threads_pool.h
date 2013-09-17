#ifndef HANDLER_THREADS_QUEUE_H
# define HANDLER_THREADS_QUEUE_H

#include <stdio.h>
#include <pthread.h>

#include "requests_queue.h"
#include "handler_thread.h"


struct handler_thread
{
    pthread_t thread;
    int       thr_id;
    struct handler_thread* next;
};

struct handler_threads_pool
{
    struct handler_thread* threads;
    struct handler_thread* last_thread;
    int num_threads;
    int max_thr_id;
    pthread_mutex_t* p_mutex;
    pthread_cond_t*  p_cond_var;
    Requests_queue* requests;
};


extern struct handler_threads_pool*
init_handler_threads_pool(pthread_mutex_t* p_mutex,
			  pthread_cond_t*  p_cond_var,
			  Requests_queue* requests);


extern void add_handler_thread(struct handler_threads_pool* pool);

extern void delete_handler_thread(struct handler_threads_pool* pool);

extern int get_handler_threads_number(struct handler_threads_pool* pool);

extern void delete_handler_threads_pool(struct handler_threads_pool* pool);

#endif /* HANDLER_THREADS_QUEUE_H */