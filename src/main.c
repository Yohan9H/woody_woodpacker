#include "woody.h"

void    verif_file(data *db)
{
    if (db->header->e_ident[EI_MAG0] != ELFMAG0 || db->header->e_ident[EI_MAG1] != ELFMAG1 || db->header->e_ident[EI_MAG2] != ELFMAG2 || db->header->e_ident[EI_MAG3] != ELFMAG3)
        exit_clean(db, "Need ELF file.", EXIT_FAILURE);

    if (db->header->e_ident[EI_CLASS] && db->header->e_ident[EI_CLASS] != 2)
        exit_clean(db, "File architecture not supported. x86_64 only", EXIT_FAILURE);
}

void    search_segement_pt_note(data *db)
{
    uint16_t    ph_num = db->header->e_phnum;
    uint16_t    ph_size = db->header->e_phentsize;
    uint8_t     *ph_table_ptr = (uint8_t *)db->header + db->header->e_phoff;
    Elf64_Phdr  *phdr;

    for (int i = 0; i < ph_num; i++)
    {
        phdr = (Elf64_Phdr *)(ph_table_ptr + (i * ph_size));
        if (phdr->p_type == PT_NOTE)
        {
            db->seg_note = phdr;
            return;
        }
    }
    exit_clean(db, "No NOTE segment found.", EXIT_FAILURE);
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
            db->crypt_seg = phdr;
            return;
        }
    }
    exit_clean(db, "No LOAD segment found.", EXIT_FAILURE);
}

void    transform_note_to_load(data *db)
{
    db->seg_note->p_type = PT_LOAD;
    db->seg_note->p_flags = PF_R | PF_X;
    db->seg_note->p_offset = db->length_file;
    db->seg_note->p_align = 0x1000;
    db->seg_note->p_filesz = 512;
    db->seg_note->p_memsz = 512;
    db->seg_note->p_vaddr = 0xc000000 + db->length_file;
    db->seg_note->p_paddr = db->seg_note->p_vaddr;

    db->cp_e_entry_ov = db->header->e_entry;
    db->header->e_entry = db->seg_note->p_vaddr;
}

void    patch_value(data *db, void *ptr, size_t size, uint64_t pattern, uint64_t replacement)
{
    unsigned char *p = (unsigned char *)ptr;
    int found = 0;

    for (size_t i = 0; i <= size - 8; i++)
    {
        if (*(uint64_t *)(p + i) == pattern)
        {
            *(uint64_t *)(p + i) = replacement;
            found++;
        }
    }
    if (found == 0)
    {
        printf("Error: Pattern %016lx not found in stub.\n", pattern);
        exit_clean(db, "Patch value failed.", EXIT_FAILURE);
    }
}

uint64_t generate_key(data *db)
{
    uint64_t key;
    int      fd_rand;

    fd_rand = open("/dev/urandom", O_RDONLY);
    if (fd_rand == -1)
        exit_clean(db, "Open /dev/urandom failed.", EXIT_FAILURE);
    if (read(fd_rand, &key, 8) != 8)
    {
        close(fd_rand);
        exit_clean(db, "Read /dev/urandom failed.", EXIT_FAILURE);
    }
    close(fd_rand);
    return (key);
}

int main(int ac, char **av)
{
    data db;
    db.fd = -1;
    db.fd_stub = -1;
    db.fd_woody = -1;
    db.length_file = 0;
    db.header = NULL;
    db.seg_note = NULL;
    db.crypt_seg = NULL;
    db.cp_e_entry_ov = 0;
    db.key = 0;

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
    
    verif_file(&db);
    search_segement_pt_note(&db);
    transform_note_to_load(&db);

    search_segement_pt_load(&db);
    db.key = generate_key(&db);
    printf("key_value: %016lX\n", db.key);
    rc4_crypt((uint8_t *)db.header + db.crypt_seg->p_offset, db.crypt_seg->p_filesz, (uint8_t *)&db.key, 8);
    
    db.fd_woody = open("woody", O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (db.fd_woody == -1)
        exit_clean(&db, "create file woody failed.", EXIT_FAILURE);
    write(db.fd_woody, db.header, db.length_file);
    db.fd_stub = open("stub.bin", O_RDONLY);
    if (db.fd_stub == -1)
        exit_clean(&db, "Open stub file failed.", EXIT_FAILURE);
    char buffer[512] = {0};
    read(db.fd_stub, buffer, 512);
    patch_value(&db, buffer, 512, 0x1111222233334444, db.seg_note->p_vaddr);
    patch_value(&db, buffer, 512, 0x5555666677778888, db.cp_e_entry_ov);
    patch_value(&db, buffer, 512, 0x1122334455667788, db.crypt_seg->p_vaddr);
    patch_value(&db, buffer, 512, 0xDEADBEEFCAFEBABE, db.crypt_seg->p_memsz);
    patch_value(&db, buffer, 512, 0x0102030405060708, db.key);
    write(db.fd_woody, buffer, 512);
    

    exit_clean(&db, NULL, EXIT_SUCCESS);
}