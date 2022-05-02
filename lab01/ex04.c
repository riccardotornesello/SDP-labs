/*
Exercise 04
Code implementation with processes
----------------------------------

Write a C program that receives two integer values on the command line,
h and n, and it generates a process tree of height h and degree n.

For example, if h=2 and n=2:
- the main process creates two processes
- each one of these 2 processes creates other two processes
- each one of these 4 processes creates other two processes
at this point, 8 leaf processes run and the program must stop.

More in detail each node of the tree is a process.
The initial process generates n child jobs and ends.
The same thing must be done by all child processes, generating a
number of processes on the leaves of the tree equal to n^h.
Processes on the leaves must all display their own PID and end.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int n, h, i, j;

    if (argc != 3)
    {
        fprintf(stderr, "Two arguments required\n");
        return -1;
    }

    n = atoi(argv[1]);
    h = atoi(argv[2]);

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (fork() == 0)
            {
                break;
            }
        }
    }

    printf("pid: %d\n", getpid());
    return 0;
}
