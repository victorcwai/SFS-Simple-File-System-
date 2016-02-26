# SFS-Simple-File-System-
System Programming AS1 a simple file system in C with a simple shell

###I don't have many time, so comments are rare in the code.

##Assumption/note:
- 1 file at most has 2 direct data block and 1 indirect data block (size = 4096+4096+4096*4096/16 = 1056768)
- didnt update i_size of directory file
- if user creates a file, file with the same name will be overwritten, no matter it is file or directory
    however, the data blocks are not overwritten immediately, it will be left alone until something is written on it
- cd_t does not create child process becoz it needs to update the currecnt working directory in parent process
- absolute path example: /abc/xyz
- root directory does not have directory mapping to self and parent
- use "cd_t root" to go to root directory
- assume the file I/O  will be printed in string format

####TODO:
- [x] mkfs_t.c: create superblock, root directory
- [x] open_t.c: get inode#/create inode + create/overwrite file/dir
- [x] ls_t
- [x] mkdir_t
- [x] external_cp
- [x] write_t.c
- [x] read_t.c
- [x] cat_t
- [x] cd_t
- [x] cp_t
- [x] tshell.c

##step:
- finish mkfs_t.c
- finish open_t.c
- finish read_t.c
- finish write_t.c
- make open_t, read_t, write_t into 3 header files
- work on tshell.c
- finish ls_t
- finish cd_t
- finish external_cp
- finish cat_t
- finish cp_t
- finish tshell.c

###Function included:
- int open_t( const char *pathname, int flags);
- struct superblock getSuperBlock();
- struct inode getInode(int inode_number);
- int createInode(char* name, struct inode parentInode, int parentInodeNum, int next_available_inode, int next_available_blk, int flags);
- void createMapping(char* name, int inodeNum, struct inode parentInode, int parentInodeNum);
- void replaceFile(int inode_number,int flags);
- int read_t( int inode_number, int offset, void *buf, int count);
- int write_t( int inode_number, int offset, void *buf, int count);

read_t:
// If offset is at or past the end of file, no bytes are read, and read_t() returns zero. 
// On success, the number of bytes read is returned (zero indicates end of file), and on error, -1 is returned.

write_t:
// The number of bytes written may be less than count if there is insufficient
// space or the maximum size of a file has been
// achieved. On success, the number of bytes written is returned (zero indicates
// nothing was written). On error, -1 is returned.
