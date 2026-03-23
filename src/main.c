#include "woody.h"

void    verif_file(data *db)
{


    if (db->header->e_ident[4] && db->header->e_ident[4] != 2)
        exit_clean(db, "File architecture not supported. x86_64 only", EXIT_FAILURE);
}

int main(int ac, char **av)
{
    data db;
    db.fd = 0;
    db.lenght_file = 0;
    db.header = NULL;

    if (ac != 2)
        exit_clean(&db, "Need one params", EXIT_FAILURE);
    db.fd = open(av[1], O_RDONLY); // A voir si changer en RDWR pour version finale
    if (db.fd == -1)
        exit_clean(&db, "Check your file params", EXIT_FAILURE);

    db.lenght_file = lseek(db.fd, 0, SEEK_END);
    if (db.lenght_file <= 0)
        exit_clean(&db, "Calcul lenght file failed", EXIT_FAILURE);
    lseek(db.fd, 0, SEEK_SET); //arret

    void *ptr;
    ptr = mmap(NULL, db.lenght_file, PROT_READ | PROT_WRITE, MAP_PRIVATE, db.fd, 0);
    if (ptr == MAP_FAILED)
        exit_clean(&db, "mmap failed", EXIT_FAILURE);
    db.header = (Elf64_Ehdr *)ptr;
    
    // print_elf_header(db.header);

    verif_file(&db);

    exit_clean(&db, NULL, EXIT_SUCCESS);
}