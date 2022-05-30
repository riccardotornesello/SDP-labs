/*
Exercise 01 (A, B, and C)
UNIX IPC
-------------------------

Two processes P1 and P2 want to exchange a message (more precisely a text) in both
directions, such that what is written by P1 is read by P2 and vice-versa.
P1 and P2 want to transfer the text between them using a piece of shared memory.
To be able to read and write the same chunk of shared memory, P1 and P2 synchronize
their R/W operations using one or more pipes.

More specifically, when P1 starts, it:
- Randomly generates a constant value n.
- Uses this value n as the size of the text to transfer to the process P1.
  - If n is zero, it stops and it forces P2 to stop too.
  - If n is not zero
    - P1 randomly generates a text including n small letters, spaces, and
      newlines.
    - P1 wakes up P2 using a pipe (e.g., it transfers on a pipe the
      text size n).
- Waits for P2.
- When it is woken up by P2, it displays the text generated by P1 converting
  all small letters into capital letters.
  No modifications have to be made to spaces and newlines.
- Repeat the entire process until either P1 or P2 generates a message (text)
  of size zero.

P2 acts as P1, but it starts by waiting and then it generates the text to
transfer to P1 (and, possibly, it uses another pipe).

Optional
--------

Version B: Use FIFOs instead of PIPEs.
Version C: Use Message Queues instead of PIPEs.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/msg.h>

#define MAX 100

typedef struct
{
    long int mtype;
    int n;
} message_t;

int sendRandom(char *r)
{
    static char charset[] = "abcdefghijklmnopqrstuvwxyz \n";
    int n = rand() % (MAX + 1); // From 0 to MAX

    for (int i = 0; i < n; i++)
    {
        int key = rand() % (int)(sizeof(charset) - 1);
        r[i] = charset[key];
    }

    return n;
}

int display(char *r, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (r[i] == ' ' || r[i] == '\n')
            printf("%c", r[i]);
        else
            printf("%c", r[i] - 32);
    }
}

void P1()
{
    int key, shmid, msgid;
    char *r;
    message_t message;

    message.mtype = 1;

    if ((key = ftok(".", 3)) == -1)
    {
        fprintf(stderr, "Error in ftok.\n");
        exit(1);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);

    if ((shmid = shmget(key, MAX, 0644 | IPC_CREAT)) == -1)
    {
        fprintf(stderr, "Error in shmid.\n");
        exit(1);
    }

    r = shmat(shmid, NULL, 0);
    if (r == (char *)(-1))
    {
        fprintf(stderr, "Error in shmat.\n");
        exit(1);
    }

    while (1)
    {
        message.n = sendRandom(r);
        msgsnd(msgid, &message, sizeof(int), 0);
        if (message.n == 0)
            break;

        msgrcv(msgid, &message, sizeof(int), 1, 0);
        if (message.n == 0)
            break;

        display(r, message.n);
    }

    msgctl(msgid, IPC_RMID, NULL);
    return;
}

void P2()
{
    int key, shmid, msgid;
    char *r;
    message_t message;

    message.mtype = 1;

    if ((key = ftok(".", 3)) == -1)
    {
        fprintf(stderr, "Error in ftok.\n");
        exit(1);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);

    if ((shmid = shmget(key, MAX, 0644 | IPC_CREAT)) == -1)
    {
        fprintf(stderr, "Error in shmid.\n");
        exit(1);
    }

    r = shmat(shmid, NULL, 0);
    if (r == (char *)(-1))
    {
        fprintf(stderr, "Error in shmat.\n");
        exit(1);
    }

    while (1)
    {
        msgrcv(msgid, &message, sizeof(int), 1, 0);
        if (message.n == 0)
            break;

        display(r, message.n);

        message.n = sendRandom(r);
        msgsnd(msgid, &message, sizeof(int), 0);
        if (message.n == 0)
            break;
    }

    msgctl(msgid, IPC_RMID, NULL);
    return;
}

int main(int argc, char *argv[])
{
    setbuf(stdout, 0);
    setbuf(stderr, 0);

    if (fork())
    {
        P1();
    }
    else
    {
        P2();
    }

    wait(0);

    return 0;
}