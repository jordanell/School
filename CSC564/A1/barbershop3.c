/*
* General solution borrowed from:
* http://en.wikipedia.org/wiki/Sleeping_barber_problem
*
* All code was written from scratch by Jordan Ell.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *barber_entry(void *ptr);
void *customer_entry(void *ptr);

sem_t barber;
sem_t seats;
sem_t customers;

int freeseats = 5;

main()
{
  // Initialize semaphores
  sem_init(&barber,     0, 0);
  sem_init(&seats,      0, 1);
  sem_init(&customers,  0, 0);

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
    sem_wait(&customers);
    sem_wait(&seats);
    freeseats++;
    printf("Cutting\n");
    printf("Customers Waiting: %d/%d\n", 5-freeseats, 5);
    sem_post(&barber);
    sem_post(&seats);
  }
}

void *customer_entry(void *ptr)
{
  while(1)
  {
    sem_wait(&seats);
    if(freeseats > 0)
    {
      freeseats--;
      printf("Customer added to queue\n");
      printf("Customers Waiting: %d/%d\n", 5-freeseats, 5);
      sem_post(&customers);
      sem_post(&seats);
      sem_wait(&barber);
    }
    else
    {
      printf("Customer turned away\n");
      sem_post(&seats);
    }
  }
}


