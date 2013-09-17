#include <stdlib.h>      /* malloc() and free()                       */
#include <assert.h>      /* assert()                                  */

#include "requests_queue.h"      /* requests queue functions and structs */

//---------------------------------------------------------------------------

Requests_queue::Requests_queue(pthread_mutex_t* p_mutex, pthread_cond_t*  p_cond_var)
{
    requests = NULL;
    last_request = NULL;
    num_requests = 0;
    m_pMutex = p_mutex;
    m_pCond_var = p_cond_var;
}
//---------------------------------------------------------------------------

Requests_queue::~Requests_queue()
{
    struct request* a_request;

    while (num_requests > 0)
    {
	a_request = get_request();
	free(a_request);
    }
}
//---------------------------------------------------------------------------
 
void Requests_queue::add_request(int client_sock)
{
    int rc;
    struct request* a_request;

    a_request = (struct request*)malloc(sizeof(struct request));
    if (!a_request)
    {
	fprintf(stderr, "add_request: out of memory\n");
	exit(1);
    }
    a_request->number = num_requests;
    a_request->next = NULL;
    a_request->client_sock = client_sock;

    rc = pthread_mutex_lock(m_pMutex);

    if (num_requests == 0)
    {
	requests = a_request;
	last_request = a_request;
    }
    else
    {
	last_request->next = a_request;
	last_request = a_request;
    }
    num_requests++;

    printf("add_request: added request with id '%d'\n", a_request->number);
    fflush(stdout);
    rc = pthread_mutex_unlock(m_pMutex);
    rc = pthread_cond_signal(m_pCond_var);
}
//---------------------------------------------------------------------------

struct request* Requests_queue::get_request()
{
    int rc;
    struct request* a_request;

    rc = pthread_mutex_lock(m_pMutex);

    if (num_requests > 0)
    {
        a_request = requests;
        requests = a_request->next;
	if (requests == NULL)
	    last_request = NULL;
	num_requests--;
    }
    else
	a_request = NULL;

    rc = pthread_mutex_unlock(m_pMutex);
    return a_request;
}
//---------------------------------------------------------------------------

int Requests_queue::get_requests_number()
{
    int rc;
    int num;

    rc = pthread_mutex_lock(m_pMutex);
    num = num_requests;
    rc = pthread_mutex_unlock(m_pMutex);
    return num;
}
//---------------------------------------------------------------------------
/*
void Requests_queue::delete_requests_queue()
{
    struct request* a_request;

    while (num_requests > 0)
    {
	a_request = get_request();
	free(a_request);
    }

    free(queue);
}*/
//---------------------------------------------------------------------------
