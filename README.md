# SFS-Simple-File-System-
COMP3438 AS1 a simple file system in C with a simple shell

####TODO:
- [x] mkfs_t.c: create superblock, root directory
- [ ] open_t.c: get inode#, create/overwrite file/dir
- [ ] ls_t.c
- [ ] mkdir_t.c
- [ ] external_cp.c
- [ ] write_t.c
- [ ] read_t.c
- [ ] cat_t.c
- [ ] cd_t.c
- [ ] cp_t.c
- [ ] tshell.c

##Note to self:
- each dir_mapping in a directory map to 1 file/dir inside it
    a directory has at least 2 mapping: ".", ".." (itself + parent), except root dir
- open_t: handle flag
- dir_mapping is stored in data block of that directory, sizeof(dir_mapping) is 16.
- remember to update meta data everytime
