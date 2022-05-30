/*
Exercise 02
Concurrent file sorting and merging
-----------------------------------

A file stores integer values in binary format on 32 bits.
The first integer stored in the file indicates the number of values
following the first one.
For example, the following three lines specify (obviously in ASCII
format, to be user-readable) the content of three possible files
(that are stored in, or must be converted into, binary format):

File 1: 5 23 45 67 9 23
File 2: 12 90 65 34 24 12 0 89 29 54 12 78 3
File 3: 3 9 5 1

Write an application which:

- Receives a variable number of strings, let us say n strings, on the
  command line.
  The first (n-1) strings are input file names.
  The last string is an output file name.
  Each input file has the format previously described.

- Runs one thread for each input file passing to each of them one of
  the input file names.
  We will refer to these (n-1) working threads as "ordering" threads.

- After running all ordering threads, the main application awaits for
  the termination of all of them.

- When the main thread waits, each ordering thread:
  - opens "its own" input file
  - reads the first integer value
  - allocates a dynamic array of integers to store all other integers
    numbers stored in the file
  - read those numbers into the array
  - orders the array (in ascending order) using whatever ordering
    algorithm it is deemed appropriate
  - ends (returning the control to the main application thread).

- The main application thread, once collected the termination of all
  ordering threads, merges all ordered arrays into a unique array,
  i.e., a unique sequence of integers.

- It stores the final ordered array into the output file, using the
  same (binary) format of the input files.

For the previous input files the output file (again, in its ASCII
version) should be:
20 0 1 3 5 9 9 12 15 23 23 24 29 34 45 54 65 67 78 89 90
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
  char *file_name;
  int n, *numbers;
} thread_data;

int threads_count, numbers_count, *final_array, final_index;
pthread_t *th;
thread_data *td;
FILE *of;

void *thread_logic(void *arg)
{
  FILE *f;
  int i, j, tmp;
  thread_data *td;

  td = (thread_data *)arg;
  f = fopen(td->file_name, "r");

  fread(&td->n, sizeof(int), 1, f);
  td->numbers = malloc(td->n * sizeof(int));

  for (int i = 0; i < td->n; i++)
  {
    fread(&td->numbers[i], sizeof(int), 1, f);
  }

  for (i = 0; i < td->n; i++)
  {
    for (j = i + 1; j < td->n; ++j)
    {
      if (td->numbers[i] > td->numbers[j])
      {
        tmp = td->numbers[i];
        td->numbers[i] = td->numbers[j];
        td->numbers[j] = tmp;
      }
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  setbuf(stdout, 0);

  if (argc < 3)
  {
    printf("At least two arguments are required\n");
    return 1;
  }

  threads_count = argc - 2;
  th = malloc(threads_count * sizeof(pthread_t));
  td = malloc(threads_count * sizeof(thread_data));
  numbers_count = 0;
  final_index = 0;

  for (int i = 0; i < threads_count; i++)
  {
    td[i].file_name = argv[i + 1];
    pthread_create(&th[i], NULL, thread_logic, (void *)&td[i]);
  }

  for (int i = 0; i < threads_count; i++)
  {
    pthread_join(th[i], NULL);
    numbers_count += td[i].n;
  }

  final_array = malloc(numbers_count * sizeof(int));
  for (int i = 0; i < threads_count; i++)
  {
    for (int j = 0; j < td[i].n; j++)
    {
      final_array[final_index] = td[i].numbers[j];
      final_index++;
    }
  }

  for (int i = 0; i < numbers_count; i++)
  {
    for (int j = i + 1; j < numbers_count; ++j)
    {
      if (final_array[i] > final_array[j])
      {
        int tmp = final_array[i];
        final_array[i] = final_array[j];
        final_array[j] = tmp;
      }
    }
  }

  of = fopen(argv[argc - 1], "w");
  fwrite(&numbers_count, sizeof(int), 1, of);
  for (int i = 0; i < numbers_count; i++)
  {
    fwrite(&final_array[i], sizeof(int), 1, of);
  }
  fclose(of);

  of = fopen(argv[argc - 1], "r");
  int nnn;
  while(fread(&nnn, sizeof(int), 1, of) > 0)
  {
    printf("%d ", nnn);
  }
  printf("\n");
}