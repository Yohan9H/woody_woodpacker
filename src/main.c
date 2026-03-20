#include "woody.h"

void    verif_file(data db)
{
    (void)db; // arret
}

int main(int ac, char **av)
{
    data db;
    db.fd = 0;
    db.lenght_file = 0;
    db.header = NULL;

    if (ac != 2)
        exit_clean(db, "*ERROR* -> need one params", EXIT_FAILURE);
    db.fd = open(av[1], O_RDONLY);
    if (db.fd == -1)
        exit_clean(db, "*ERROR* -> check your file params", EXIT_FAILURE);

    db.lenght_file = lseek(db.fd, 0, SEEK_END);
    if (db.lenght_file <= 0)
        exit_clean(db, "*ERROR* -> calcul lenght file failed", EXIT_FAILURE);
    lseek(db.fd, 0, SEEK_SET);

    void *ptr;
    ptr = mmap(NULL, db.lenght_file, PROT_READ | PROT_WRITE, MAP_PRIVATE, db.fd, 0);
    if (ptr == MAP_FAILED)
        exit_clean(db, "*ERROR* -> mmap failed", EXIT_FAILURE);
    db.header = (Elf64_Ehdr *)ptr;
    
    print_elf_header(db.header);

    verif_file(db);

    exit_clean(db, NULL, EXIT_SUCCESS);
}