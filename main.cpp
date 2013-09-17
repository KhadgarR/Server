#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>

#include "requests_queue.h"
#include "handler_thread.h"
#include "handler_threads_pool.h"

#define NUM_HANDLER_THREADS 3
#define MAX_NUM_HANDLER_THREADS 15

#define HIGH_REQUESTS_WATERMARK 15
#define LOW_REQUESTS_WATERMARK 3

pthread_mutex_t request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_cond_t  got_request   = PTHREAD_COND_INITIALIZER;

int done_creating_requests = 0;

int main(int argc, char* argv[])
{
  struct handler_threads_pool* handler_threads = NULL;
  int socket_desc , client_sock , len, i;
  struct sockaddr_in server, client;

  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
      printf("Could not create socket");
   
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( 8888 );
   
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("bind failed. Error");
      return 1;
  }
   
  listen(socket_desc , 3);

  len = sizeof(struct sockaddr_in);

  Requests_queue* requests = new Requests_queue(&request_mutex, &got_request);
  assert(requests);

  handler_threads = init_handler_threads_pool(&request_mutex, &got_request, requests);
  assert(handler_threads);

  int num_requests; // количество запросов которых необходимо обработать
  int num_threads;  // количество активных потоков

  //TODO: выделить отдельный поток под прослушивание сокета и не загружать основной,
  //в мэин потоке будет выводить лишь статистика по подключениям.
  //изменится основной цикл, будет использоваться epoll
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&len)) )
  {
    requests->add_request(client_sock);

    num_requests = requests->get_requests_number();
    num_threads = get_handler_threads_number(handler_threads);

    if (num_requests > HIGH_REQUESTS_WATERMARK &&
        num_threads < MAX_NUM_HANDLER_THREADS)
    {
      printf("main: adding thread: '%d' requests, '%d' threads\n",
      num_requests, num_threads);
      add_handler_thread(handler_threads);
    }
    if (num_requests < LOW_REQUESTS_WATERMARK && num_threads > NUM_HANDLER_THREADS)
    {
        printf("main: deleting thread: '%d' requests, '%d' threads\n",
    	   num_requests, num_threads);
        delete_handler_thread(handler_threads);
    }
  }

  delete_handler_threads_pool(handler_threads);
  delete requests;
    
  printf("we are done.\n");
  return 0;
}