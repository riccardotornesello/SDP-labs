/*
Exercise 01
Synchronization with barriers
-----------------------------

Given an array v of 2^n elements v[i], i.e.,

v = { v[0], v[1], ... , v[2^n − 1] }

its k-th prefix sum (the cumulative sum vector) is the sum of its first k+1
elements, i.e.,
v[0] + v[1] + ... + v[k]

Write a C program using Pthreads that:
* Receives an integer value n as an argument of the command line
* Randomly generates an array v of 2^n integer elements in the range [1-9]
* Computes the prefix sum of v as follows.

For an array v of 2^n elements, it is possible to compute all values of
the prefix sum in n steps, by means of 2^n−1 concurrent threads, each
associated with an array element.

For example for n=3 and 2^3 = 8 elements, each step consists of each
thread taking two elements, and computing a result:
* In the first step, each thread computes the sums of adjacent elements
  (i.e., gap = 1=2^0).
* In the second step, each thread computes the sums of the elements
  that are 2 elements away (i.e., gap = 2 = 2^1).
* In the third step, each thread computes the sums of the elements
  that are 4 elements away (i.e., gap = 4 = 2^2).
After 3 steps, all sum prefix values are computed.

In general, if we have 2^n elements in v, step i computes the sums of
elements that are 2^(i-1) elements away.
After n steps, all cumulative sum values are computed.

For example, let us suppose the program is run as:

> cumulative_sum  3

It should compute (and eventually display) the following:

Initial array v:  2  4  6  1  3  5  8  7
v after step 1 :  2  6 10  7  4  8 13 15
                  Computed as: v[0]=v[0], v[1]=v[0]+v[1],
                     v[2]=v[1]+v[2], etc., v[n-1]=v[n-2]+v[n-1]
                               (sum of elements at distance 2^0=1)
v after step 2 :  2  6 12 13 14 15 17 23
                  Computed as: v[0]=v[0], v[1]=v[1], v[2]=v[0]+v[2],
                               v[3]=v[1]+v[3], etc., v[n-1]=v[n-3]+v[n-1]
                               (sum of elements at distance 2^1=2)
v after step 3 :  2  6 12 13 16 21 29 36
                  Computed as: v[0]=v[0], v[1]=v[1], v[2]=v[2], v[3]=v[4],
                               v[4]=v[0]+v[4], v[5]=v[1]+v[5], etc.,
                               v[n-1]=v[n-5]+v[n-1]
                               (sum of elements at distance 2^2=4)
There is no step 4 as in step 4 the procedure should sum elements at a
distance 2^3=8 (which is the size of the array).

Notice that in the final array:
- v[7]=36 is the sum of elements v[0]+v[1]+...+v[7]
- v[6]=29 is the sum of elements v[0]+v[1]+...+v[6]
- v[5]=21 is the sum of elements v[0]+v[1]+...+v[5]
- etc.

Moreover, notice that:
1. The main thread allocates, fills, and prints the initial content of
   the array v
2. All threads are initially created by the main thread, they have all
   the same code and they must synchronize their activity according to the
   procedure previously described, exploiting the maximum possible
   concurrency.
3. The array v must be updated in place, i.e., you cannot allocate
   auxiliary arrays.
4. Each thread must loop for no more than n times (the number of
   steps) before exiting.
5. Some threads terminate before the n-th step.
   In particular thread i terminates after step i, with i in the
   range [1–n].
6. The main thread prints immediately the value in v[0], then it waits
   the termination of each thread i, in order of creation, and
   immediately prints the value stored in v[i].

Suggestions
-----------

* First design and write the pseudo-code of a solution excluding rule 5.
* Then, update your solution including rule 5.
  Please notice that:
  - If you refer to v[i-gap], index i-gap must be non-negative
  - You must properly manage a variable that stores the number of
    active threads, which must be decremented according to rule 5.
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

int n, elements_count, threads_count, threads_done;
int *v;
pthread_t *th;
sem_t *sem_v_read, *sem_v_write, *sem_main;

void *sum_thread(void *arg)
{
    int v_index, gap, step, new_value;

    v_index = (int)arg; // From 1 to 2^n-1 (all indexes of the array except the first one)

    for (step = 0; step < n; step++)
    {
        sem_wait(sem_v_read);
        gap = 1 << step;
        if (gap > v_index)
            new_value = v[v_index];
        else
            new_value = v[v_index] + v[v_index - gap];

        sem_wait(sem_main);
        if (++threads_done == threads_count)
            for (int i = 0; i < threads_count; i++)
                sem_post(sem_v_write);
        sem_post(sem_main);

        sem_wait(sem_v_write);
        v[v_index] = new_value;

        sem_wait(sem_main);
        if (--threads_done == 0)
            for (int i = 0; i < threads_count; i++)
                sem_post(sem_v_read);
        sem_post(sem_main);
    }
    pthread_exit((void *)v_index);
}

int main(int argc, char *argv[])
{
    setbuf(stdout, 0);

    if (argc != 2)
    {
        printf("One argument required!\n");
        return 1;
    }

    n = atol(argv[1]);

    elements_count = 1 << n;
    v = malloc(sizeof(int) * elements_count);
    printf("Initial array: ");
    for (int i = 0; i < elements_count; i++)
    {
        v[i] = rand() % 10 + 1;
        printf("%d ", v[i]);
    }
    printf("\n");

    threads_done = 0;
    threads_count = elements_count - 1;

    sem_v_read = malloc(sizeof(sem_t));
    sem_init(sem_v_read, 0, threads_count);
    sem_v_write = malloc(sizeof(sem_t));
    sem_init(sem_v_write, 0, 0);
    sem_main = malloc(sizeof(sem_t));
    sem_init(sem_main, 0, 1);

    th = malloc(sizeof(pthread_t) * threads_count);
    for (int i = 0; i < threads_count; i++)
    {
        pthread_create(&th[i], NULL, sum_thread, (void *)(i + 1));
    }

    for (int i = 0; i < threads_count; i++)
    {
        int returned_value;
        pthread_join(th[i], (void *)&returned_value);
    }

    printf("Final array: ");
    for (int i = 0; i < elements_count; i++)
    {
        printf("%d ", v[i]);
    }
    printf("\n");
}