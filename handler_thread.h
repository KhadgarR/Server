#ifndef HANDLER_THREAD_H
# define HANDLER_THREAD_H

#include <stdio.h>
#include <pthread.h>
#include "requests_queue.h"

class HandlerThreadPparams
{
  public:
    HandlerThreadPparams(int, pthread_mutex_t*, pthread_cond_t*, Requests_queue*);
    ~HandlerThreadPparams();

//TODO: скрыть все член данные, сделать соответствующие методы.
//private:
    int thread_id;
    pthread_mutex_t* request_mutex;
    pthread_cond_t*  got_request;
    Requests_queue* requests;
};

extern void* handle_requests_loop(void* thread_params);

#endif /* HANDLER_THREAD_H */