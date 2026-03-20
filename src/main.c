#include "woody.h"

int main(int ac, char **av)
{
    data db;

    if (ac != 2)
        exit_clean(db, "*ERROR* -> need one params");
    db.fd = open(av[1], O_RDONLY);
    if (db.fd == -1)
        exit_clean(db, "*ERROR* -> check your file params");

    
}