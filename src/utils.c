#include "woody.h"

void    exit_clean(data db, char *err)
{
    if (err != NULL && errno != 0)
        perror(err);
    else
        ft_putstr_fd(err, 2);

    if (db.fd)
        close(db.fd);

    exit(1);
}