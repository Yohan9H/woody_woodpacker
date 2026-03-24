#include "woody.h"

void    verif_file(data *db)
{
    if (db->header->e_ident[EI_MAG0] != ELFMAG0 || db->header->e_ident[EI_MAG1] != ELFMAG1 || db->header->e_ident[EI_MAG2] != ELFMAG2 || db->header->e_ident[EI_MAG3] != ELFMAG3)
        exit_clean(db, "Need ELF file.", EXIT_FAILURE);

    if (db->header->e_ident[EI_CLASS] && db->header->e_ident[EI_CLASS] != 2)
        exit_clean(db, "File architecture not supported. x86_64 only", EXIT_FAILURE);
}

void    search_segement_pt_load(data *db)
{
    uint16_t    ph_num = db->header->e_phnum;
    uint16_t    ph_size = db->header->e_phentsize;
    uint8_t     *ph_table_ptr = (uint8_t *)db->header + db->header->e_phoff;
    Elf64_Phdr  *phdr;

    for (int i = 0; i < ph_num; i++)
    {
        phdr = (Elf64_Phdr *)(ph_table_ptr + (i * ph_size));
        if (phdr->p_type == PT_LOAD && (phdr->p_flags & PF_X))
        {
            db->seg_load = phdr;
            break;
        }
    }
}

int main(int ac, char **av)
{
    data db;
    db.fd = 0;
    db.length_file = 0;
    db.header = NULL;
    db.seg_load = NULL;

    if (ac != 2)
        exit_clean(&db, "Need one params.", EXIT_FAILURE);
    db.fd = open(av[1], O_RDONLY); // A voir si changer en RDWR pour version finale
    if (db.fd == -1)
        exit_clean(&db, "Check your file params.", EXIT_FAILURE);

    db.length_file = lseek(db.fd, 0, SEEK_END);
    if (db.length_file <= 0 || (size_t)db.length_file < sizeof(Elf64_Ehdr))
        exit_clean(&db, "Calcul length file failed.", EXIT_FAILURE);
    lseek(db.fd, 0, SEEK_SET);

    void *ptr;
    ptr = mmap(NULL, db.length_file, PROT_READ | PROT_WRITE, MAP_PRIVATE, db.fd, 0);
    if (ptr == MAP_FAILED)
        exit_clean(&db, "mmap failed.", EXIT_FAILURE);
    db.header = (Elf64_Ehdr *)ptr;
    
    // print_elf_header(db.header);

    verif_file(&db);
    search_segement_pt_load(&db);
    if (db.seg_load == NULL)
        exit_clean(&db, "No PT_LOAD in your file.", EXIT_FAILURE);
    
    printf("%d", db.seg_load->p_type);



    exit_clean(&db, NULL, EXIT_SUCCESS);
}