#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include "sfsheader.h"

int read_t( int inode_number, int offset, void *buf, int count);
//not yet do direct block2, indirect block
int read_t( int inode_number, int offset, void *buf, int count)
{
	struct inode inode;
	inode = getInode(inode_number);
	if(inode.i_number != inode_number) return -1; //error
	if(offset>=inode.i_size) 
	{
		printf("Offset is at or past the end of file, no bytes are read.\n");
		return 0;
	}
	dirDataBlkIndex = inode.direct_blk[0];
	int fd = open ("HD", O_RDWR, 660);
	lseek(fd, DATA_OFFSET + dirDataBlkIndex * BLOCK_SIZE + offset, SEEK_SET);
	read(fd, buf, count);
	if(count>=inode.i_size)
	{
		//read only up to count
		return inode.i_size;
	}else
		return count;
}

// If offset is at or past the end of file, no bytes are read, and read_t() returns zero. 
// On success, the number of bytes read is returned (zero indicates end of file), and on error, -1 is returned.