#ifndef REQUESTS_QUEUE_H
# define REQUESTS_QUEUE_H

#include <stdio.h>       /* standard I/O routines                     */
#include <pthread.h>     /* pthread functions and data structures     */
#include <sys/socket.h>

/* format of a single request. */
struct request
{
    int number;		    /* number of the request                  */
    struct request* next;   /* pointer to next request, NULL if none. */
    int client_sock;
};


class Requests_queue
{
  public:
    Requests_queue(pthread_mutex_t* p_mutex, pthread_cond_t*  p_cond_var);
    ~Requests_queue();
 
    void add_request(int client_sock);
    struct request* get_request();
    int get_requests_number();
    //void delete_requests_queue();

  private:
    struct request* requests;
    struct request* last_request;
    int num_requests;
    pthread_mutex_t* m_pMutex;
    pthread_cond_t*  m_pCond_var;
};
#endif /* REQUESTS_QUEUE_H */