#ifndef WOODY_H
#define WOODY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../libft/libft.h"

typedef struct data
{
    int fd;
} data;

void    exit_clean(data db, char *err);

#endif