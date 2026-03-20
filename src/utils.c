#include "woody.h"

void    exit_clean(data db, char *err)
{
    if (err != NULL)
        perror(err);
    close(db.fd);
}