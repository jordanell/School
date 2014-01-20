/*
* General algorithm borrowed from:
* http://www.cs.cornell.edu/courses/cs4410/2008fa/oldexams/final-solutions.pdf
*
* All code was written from scratch by Jordan Ell.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *h_entry(void *ptr);
void *o_entry(void *ptr);
void bond();

sem_t oxygen;
sem_t hydrogen;

int h_count = 0;
int water = 0;

main()
{
  // Initialize semaphores
  sem_init(&oxygen,     0, 0);
  sem_init(&hydrogen,   0, 0);

  pthread_t thread1, thread2, thread3, thread4;
  int iret1, iret2, iret3, iret4;

  iret1 = pthread_create( &thread1, NULL, h_entry, NULL);
  iret2 = pthread_create( &thread2, NULL, h_entry, NULL);
  iret3 = pthread_create( &thread3, NULL, o_entry, NULL);
  iret4 = pthread_create( &thread4, NULL, o_entry, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_join(thread4, NULL);

  exit(0);
}

void *h_entry(void *ptr)
{
  while(1)
  {
    h_count++;
    if(h_count % 2 == 1)
    {
      sem_post(&hydrogen);
    }
    else
    {
      sem_post(&oxygen);
      sem_wait(&hydrogen);
    }
  }
}

void *o_entry(void *ptr)
{
  while(1)
  {
    sem_wait(&oxygen);
    bond();
    sem_post(&hydrogen);
    sem_post(&hydrogen);
  }
}

void bond()
{
  water++;
  printf("Bonded: %d\n", water);
}


