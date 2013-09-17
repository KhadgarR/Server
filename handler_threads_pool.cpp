#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

#include "handler_threads_pool.h"

struct handler_threads_pool*
init_handler_threads_pool(pthread_mutex_t* p_mutex,
			  pthread_cond_t*  p_cond_var,
			  Requests_queue* requests)
{
    struct handler_threads_pool* pool =
      (struct handler_threads_pool*)malloc(sizeof(struct handler_threads_pool));

    if (!pool)
    {
	fprintf(stderr, "init_handler_threads_pool: out of memory. exiting\n");
	exit(1);
    }

    pool->threads = NULL;
    pool->last_thread = NULL;
    pool->num_threads = 0;
    pool->max_thr_id = 0;
    pool->p_mutex = p_mutex;
    pool->p_cond_var = p_cond_var;
    pool->requests = requests;

    return pool;
}

void add_handler_thread(struct handler_threads_pool* pool)
{
    struct handler_thread* a_thread;

    assert(pool);

    a_thread = (struct handler_thread*)malloc(sizeof(struct handler_thread));
    if (!a_thread)
    {
	fprintf(stderr, "add_handler_thread: out of memory. exiting\n");
	exit(1);
    }
    a_thread->thr_id = pool->max_thr_id++;
    a_thread->next = NULL;

    HandlerThreadPparams* params = new HandlerThreadPparams(a_thread->thr_id, pool->p_mutex, pool->p_cond_var, pool->requests);

    if (!params)
    {
	fprintf(stderr, "add_handler_thread: out of memory. exiting\n");
	exit(1);
    }

    pthread_create(&a_thread->thread,
		   NULL,
		   handle_requests_loop,
		   (void*)params);

    if (pool->num_threads == 0)
    {
        pool->threads = a_thread;
        pool->last_thread = a_thread;
    }
    else
    {
        pool->last_thread->next = a_thread;
        pool->last_thread = a_thread;
    }
    pool->num_threads++;
}


static struct handler_thread* remove_first_handler_thread(struct handler_threads_pool* pool)
{
    struct handler_thread* a_thread = NULL;
    assert(pool);

    if (pool->num_threads > 0 && pool->threads)
    {
	a_thread = pool->threads;
	pool->threads = a_thread->next;
	a_thread->next = NULL;
        pool->num_threads--;
    }

    return a_thread;
}

void delete_handler_thread(struct handler_threads_pool* pool)
{
    struct handler_thread* a_thread;
    assert(pool);

    a_thread = remove_first_handler_thread(pool);
    if (a_thread)
    {
	pthread_cancel(a_thread->thread);
        free(a_thread);
    }
}

int get_handler_threads_number(struct handler_threads_pool* pool)
{
    assert(pool);
    return pool->num_threads;
}

void delete_handler_threads_pool(struct handler_threads_pool* pool)
{
    void* thr_retval;
    struct handler_thread* a_thread;

    assert(pool);

    while (pool->num_threads > 0)
    {
	a_thread = remove_first_handler_thread(pool);
	assert(a_thread);
	pthread_join(a_thread->thread, &thr_retval);
        free(a_thread);
    }
}
