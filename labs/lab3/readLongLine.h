/*
 * Description:
 * This program will act as a filter, removing
 * adjacent duplicate lines as it copies its stdin to its stdout. That is, any line that is identical
 * to the previous line will be discarded rather than copied to stdout.
 * Your program may not impose any limits on file size or line length.
 * To get started, I highly recommend writing a function unsigned char *read long line(FILE *file)
 * that will read an arbitrarily long line from the given file into newly-allocated space. Once you
 * have that, the program is easy.
 * Be careful to free memory once you are done with it. A memory leak could be a real problem
 * for a program like this.
 *
 */
#ifndef READLONGLINE_H
#define READLONGLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define MAXCHAR 524 /*inital amt of lines in the buffer*/

void *safe_calloc(size_t nmemb, int size);
void *safe_realloc(void *src, int size);
void safe_free(char **buff);
void safe_close(int *fd);

char *read_long_line(int inFd, int max_chars);
#endif
