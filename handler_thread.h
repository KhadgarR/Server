#ifndef HANDLER_THREAD_H
# define HANDLER_THREAD_H

#include <stdio.h>       /* standard I/O routines                     */
#include <pthread.h>     /* pthread functions and data structures     */
#include "requests_queue.h"   /* requests queue routines/structs      */

class HandlerThreadPparams
{
  public:
    HandlerThreadPparams(int, pthread_mutex_t*, pthread_cond_t*, Requests_queue*);
    ~HandlerThreadPparams();

  //private:
    int thread_id;
    pthread_mutex_t* request_mutex;
    pthread_cond_t*  got_request;
    Requests_queue* requests;
};
/*
struct handler_thread_params
{
    int thread_id;
    pthread_mutex_t* request_mutex;
    pthread_cond_t*  got_request;
    Requests_queue* requests;
};*/

/* a handler thread's main loop function */
extern void* handle_requests_loop(void* thread_params);

#endif /* HANDLER_THREAD_H */