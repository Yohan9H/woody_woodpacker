#include "woody.h"

void    exit_clean(data *db, char *err, int exit_code)
{
    if (err != NULL && errno != 0)
        perror(err);
    else if (err != NULL)
        ft_putstr_fd(err, 2);

    if (db->fd >= 0)
        close(db->fd);
    if (db->header >= 0)
        munmap(db->header, db->length_file);
    if (db->fd_woody >= 0)
        close(db->fd_woody);
    if (db->fd_stub >= 0)
        close(db->fd_stub);

    exit(exit_code);
}

void    print_elf_header(Elf64_Ehdr *h)
{
    if (!h)
        return;

    printf("--- ELF HEADER ---\n");
    
    printf("Magic:   ");
    for(int i = 0; i < EI_NIDENT; i++)
        printf("%02x ", h->e_ident[i]);
    printf("\n");

    printf("Type:                              %u\n", h->e_type);
    printf("Machine:                           %u\n", h->e_machine);
    printf("Version:                           %u\n", h->e_version);
    
    printf("Entry point address:               0x%lx\n", h->e_entry);
    printf("Start of program headers:          %lu (bytes into file)\n", h->e_phoff);
    printf("Start of section headers:          %lu (bytes into file)\n", h->e_shoff);
    
    printf("Flags:                             0x%x\n", h->e_flags);
    printf("Size of this header:               %u (bytes)\n", h->e_ehsize);
    printf("Size of program headers:           %u (bytes)\n", h->e_phentsize);
    printf("Number of program headers:         %u\n", h->e_phnum);
    printf("Size of section headers:           %u (bytes)\n", h->e_shentsize);
    printf("Number of section headers:         %u\n", h->e_shnum);
    printf("Section header string table index: %u\n", h->e_shstrndx);
    printf("------------------\n");
}