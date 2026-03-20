#include "woody.h"

void    exit_clean(data db, char *err, int exit_code)
{
    if (err != NULL && errno != 0)
        perror(err);
    else if (err != NULL)
        ft_putstr_fd(err, 2);

    if (db.fd)
        close(db.fd);
    if (db.header)
        munmap(db.header, db.lenght_file);

    exit(exit_code);
}

void    print_elf_header(Elf64_Ehdr *h)
{
    if (!h)
        return;

    ft_printf("--- ELF HEADER ---\n");
    
    ft_printf("Magic:   ");
    for(int i = 0; i < EI_NIDENT; i++)
        ft_printf("%02x ", h->e_ident[i]);
    ft_printf("\n");

    ft_printf("Type:                              %u\n", h->e_type);
    ft_printf("Machine:                           %u\n", h->e_machine);
    ft_printf("Version:                           %u\n", h->e_version);
    
    ft_printf("Entry point address:               0x%lx\n", h->e_entry);
    ft_printf("Start of program headers:          %lu (bytes into file)\n", h->e_phoff);
    ft_printf("Start of section headers:          %lu (bytes into file)\n", h->e_shoff);
    
    ft_printf("Flags:                             0x%x\n", h->e_flags);
    ft_printf("Size of this header:               %u (bytes)\n", h->e_ehsize);
    ft_printf("Size of program headers:           %u (bytes)\n", h->e_phentsize);
    ft_printf("Number of program headers:         %u\n", h->e_phnum);
    ft_printf("Size of section headers:           %u (bytes)\n", h->e_shentsize);
    ft_printf("Number of section headers:         %u\n", h->e_shnum);
    ft_printf("Section header string table index: %u\n", h->e_shstrndx);
    ft_printf("------------------\n");
}