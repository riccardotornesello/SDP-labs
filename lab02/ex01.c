/*
Exercise 01
Binary files
------------

An ASCII file has lines with the following format:
- An identifier, i.e., an integer value varying from 1 to the number
  of rows in the file (e.g., 1, 2, etc.)
- A register number, i.e., a long integer of 6 digits (e.g., 164678)
- A surname, i.e., a string of a maximum of 30 characters (e.g., Rossi)
- A name, i.e., a string of a maximum of 30 characters (e.g., Mario)
- An examination mark, i.e., an integer value.

The following is a correct example of such a file:

1 100000 Romano Antonio 25
2 150000 Fabrizi Aldo 22
3 200000 Verdi Giacomo 15
4 250000 Rossi Luigi 30

Write a C program in UNIX which is run with 3 file names on the
command line:
file_1  file_2  file_3
and which can:
- Read the *text* file file_1 (with the previous format) and
  store the same information into file file_2 in *binary* format.
  The binary format file must have *fixed-length* records (i.e.,
  each record must include an integer + a long integer + 31 characters
  + 31 characters + an integer).
- Read back the *binary* file file_2 and store it as a *text*
  into file file_3 (with the same format of file_1).

Suggestions
-----------
Manipulate text files using C library functions and binary files
using the I/O UNIX library.
As an alternative, use only the UNIX library *but* operate on the ASCII
file on a byte-by-byte basis (and then move forward and backward to
the record-by-record representation).

Verify that files file_input_1 and file_output_2 are indeed equivalent
using the shell command "wc" and "diff".
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  int id;
  long regNum;
  char surname[31];
  char name[31];
  int mark;
} exam_s;

int main(int argc, char *argv[])
{
  FILE *in, *out;
  exam_s exam;

  if (argc != 4)
  {
    printf("Three arguments required\n");
    return -1;
  }

  in = fopen(argv[1], "r");
  if (in == NULL)
  {
    printf("Error opening file 1\n");
    return -1;
  }

  out = fopen(argv[2], "wb");
  if (out == NULL)
  {
    printf("Error opening file 2\n");
    return -1;
  }

  while (fscanf(in, "%d %ld %s %s %d", &exam.id, &exam.regNum, exam.surname, exam.name, &exam.mark) != EOF)
  {
    fwrite(&exam, sizeof(exam), 1, out);
  }

  fclose(in);
  fclose(out);

  in = fopen(argv[2], "rb");
  if (in == NULL)
  {
    printf("Error opening file 2\n");
    return -1;
  }

  out = fopen(argv[3], "w");
  if (out == NULL)
  {
    printf("Error opening file 3\n");
    return -1;
  }

  while (fread(&exam, sizeof(exam), 1, in) > 0)
  {
    fprintf(out, "%d %ld %s %s %d\n", exam.id, exam.regNum, &exam.surname, &exam.name, exam.mark);
  }

  fclose(in);
  fclose(out);

  return 0;
}