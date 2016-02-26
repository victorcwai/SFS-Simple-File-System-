# SFS-Simple-File-System-
System Programming AS1 a simple file system in C with a simple shell

##Assumption/note:
- 1 file at most has 2 direct data block and 1 indirect data block (size = 4096+4096+4096*4096/16 = 1056768)
- didnt update i_size of directory file
- cannot create file with the same file, i.e. no overwrite

####TODO:
- [x] mkfs_t.c: create superblock, root directory
- [x] open_t.c: get inode#/create inode + create/overwrite file/dir
- [x] ls_t
- [ ] mkdir_t
- [ ] external_cp
- [x] write_t.c
- [x] read_t.c
- [ ] cat_t
- [ ] cd_t
- [ ] cp_t
- [x] tshell.c

##Note to self:


###Function included:
- int open_t( const char *pathname, int flags);
- struct superblock getSuperBlock();
- struct inode getInode(int inode_number);
- int createInode(char* name, struct inode parentInode, int parentInodeNum, int next_available_inode, int next_available_blk, int flags);
- void createMapping(char* name, int inodeNum, struct inode parentInode, int parentInodeNum);
- int read_t( int inode_number, int offset, void *buf, int count);
- int write_t( int inode_number, int offset, void *buf, int count);
- int createDirectBlk2(struct inode inode, int count);

read_t:
// If offset is at or past the end of file, no bytes are read, and read_t() returns zero. 
// On success, the number of bytes read is returned (zero indicates end of file), and on error, -1 is returned.

write_t:
// The number of bytes written may be less than count if there is insufficient
// space or the maximum size of a file has been
// achieved. On success, the number of bytes written is returned (zero indicates
// nothing was written). On error, -1 is returned.
