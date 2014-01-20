/*
* All code was written from scratch by Jordan Ell.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *barber_entry(void *ptr);
void *customer_entry(void *ptr);

pthread_mutex_t mutex;

int freeseats = 5;
int maxseats  = 5;

main()
{
  // Initialize mutex
  pthread_mutex_init(&mutex, NULL);

  pthread_t thread1, thread2;
  int iret1, iret2;

  iret1 = pthread_create( &thread1, NULL, barber_entry, NULL);
  iret2 = pthread_create( &thread2, NULL, customer_entry, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  exit(0);
}

void *barber_entry(void *ptr)
{
  while(1)
  {
    pthread_mutex_lock(&mutex);
    if(freeseats < maxseats)
    {
      freeseats++;
      printf("Cutting\n");
      printf("Customers Waiting: %d/%d\n", 5-freeseats, 5);
    }
    pthread_mutex_unlock(&mutex);
  }
}

void *customer_entry(void *ptr)
{
  while(1)
  {
    pthread_mutex_lock(&mutex);
    if(freeseats > 0)
    {
      freeseats--;
      printf("Customer added to queue\n");
      printf("Customers Waiting: %d/%d\n", 5-freeseats, 5);
    }
    else
    {
      printf("Customer turned away\n");
    }
    pthread_mutex_unlock(&mutex);
  }
}


