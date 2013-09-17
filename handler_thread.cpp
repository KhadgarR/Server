#include <stdio.h>       /* standard I/O routines                     */
#include <pthread.h>     /* pthread functions and data structures     */
#include <stdlib.h>      /* malloc() and free()                       */
#include <assert.h>      /* assert()                                  */

#include "handler_thread.h"   /* handler thread functions/structs     */

extern int done_creating_requests;   /* are we done creating new requests? */


HandlerThreadPparams::HandlerThreadPparams(int thr_id, pthread_mutex_t* p_mutex, pthread_cond_t* p_cond_var, Requests_queue* queue)
{
    thread_id = thr_id;
    request_mutex = p_mutex;
    got_request = p_cond_var;
    requests = queue;
}

HandlerThreadPparams::~HandlerThreadPparams()
{

}


static void cleanup_free_mutex(void* a_mutex)
{
  pthread_mutex_t* p_mutex = (pthread_mutex_t*)a_mutex;

  if (p_mutex)
    pthread_mutex_unlock(p_mutex);
}

static void handle_request(struct request* a_request, int thread_id)
{
  //handle
}

void* handle_requests_loop(void* thread_params)
{
  int rc;
  struct request* a_request;
  HandlerThreadPparams *data = (HandlerThreadPparams*)thread_params;
  assert(data);

  printf("Starting thread '%d'\n", data->thread_id);
  fflush(stdout);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  pthread_cleanup_push(cleanup_free_mutex, (void*)data->request_mutex);

  rc = pthread_mutex_lock(data->request_mutex);

  printf("thread '%d' after pthread_mutex_lock\n", data->thread_id);
  fflush(stdout);
  //TODO: улучшить механизм чтения, адаптировать под несколько сокетов.
  while (1)
  {
      int num_requests = data->requests->get_requests_number();

      printf("thread '%d', num_requests =  %d\n", data->thread_id, num_requests);
      fflush(stdout);

      if (num_requests > 0)
      {
        a_request = data->requests->get_request();
        if (a_request)
        {
          char client_message[5];
          int read_size = 0;
          /*TODO:временное решение, по хорошему необходимо сделать чтение того что уже есть
            и не задерживаться на данном сокете, сохранять не полные сообщения, затем
            дочитывать.
          */
          while( (read_size = recv(a_request->client_sock, client_message, 5 , 0)) > 0 )
          {
            //TODO: ответ первым байтом только в случае если прочитали 5 байт.
            //или если пришёл нулевой ключ отключаем клиента.

          }
          //TODO: удалять только если приняты все сообщения с клиента.
          free(a_request);
        }
      }
      else
      {
        pthread_mutex_unlock(data->request_mutex);
        printf("thread '%d' exiting\n", data->thread_id);
        fflush(stdout);

        pthread_exit(NULL);
        printf("thread '%d' before pthread_cond_wait\n", data->thread_id);
        fflush(stdout);

        rc = pthread_cond_wait(data->got_request, data->request_mutex);

        printf("thread '%d' after pthread_cond_wait\n", data->thread_id);
        fflush(stdout);
      }
  }
  pthread_cleanup_pop(0);
}
