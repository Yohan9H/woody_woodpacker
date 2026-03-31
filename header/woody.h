#ifndef WOODY_H
#define WOODY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>
#include <sys/mman.h>
#include "../libft/libft.h"

typedef struct data
{
    int         fd;
    int         fd_woody;
    int         fd_stub;
    off_t       length_file;
    Elf64_Ehdr  *header;
    Elf64_Phdr  *seg_note;
    Elf64_Addr  cp_e_entry_ov;
    Elf64_Phdr  *crypt_seg;
    uint64_t    key;
} data;

void    exit_clean(data *db, char *err, int exit_code);
void    print_elf_header(Elf64_Ehdr *h);

#endif