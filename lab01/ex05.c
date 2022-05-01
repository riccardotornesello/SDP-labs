/*
Exercise 05
Code implementation with processes
----------------------------------

A binary number of size n can be easily generated by a recursive function
like the one reported in file e05recursive.c

Transform the recursive program into a concurrent one, replacing the
recursive procedure with the generation of an appropriate number of
*processes*.
Each process must display one binary number.
Binary numbers can be displayed in any order.

For example, if n=3, the function must generate and print numbers:
000, 001, 010, 011, 100, 101, 110, 111
IN ANY ORDER.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

void binary(int *, int, int);

int main(int argc, char *argv[])
{
    int n, i;
    char *vet;

    n = atoi(argv[1]);

    vet = (char *)malloc(n * (sizeof(int) + 1));
    if (vet == NULL)
    {
        printf("Allocatin Error.\n");
        exit(1);
    }

    for (i = 0; i < n; i++)
    {
        if (fork())
        {
            vet[i] = '0';
        }
        else
        {
            vet[i] = '1';
        }
    }

    printf("Binary Number: %s\n", vet);

    return 0;
}