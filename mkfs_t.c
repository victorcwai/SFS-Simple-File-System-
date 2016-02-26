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

int main(int argc, char *argv[])
{
	int fd = open (argv[1], O_RDWR, 660);
	if (fd == -1)
	{
		printf("No such file. Exit.\n");
		return -1;
	}
	//0-512:BOOT, 512-4K:superblock, 4K-10M:inode, 10M-110M:data

	struct superblock* sb;
	struct inode* inode;
	struct dir_mapping* rootDir;

	//superblock region
	sb = (struct superblock*)malloc(sizeof(struct superblock));
	sb->inode_offset = INODE_OFFSET;
	sb->data_offset = DATA_OFFSET;
	sb->max_inode = MAX_INODE;
	sb->max_data_blk = MAX_DATA_BLK;
	sb->blk_size = BLOCK_SIZE;
	sb->next_available_inode = 0;
	sb->next_available_blk = 0;
	//write
	lseek(fd, SB_OFFSET, SEEK_SET); //point to SB_OFFSET = 512B
	write(fd, (void *)sb, sizeof(struct superblock));

	//create root directory inode
	inode = (struct inode*)malloc(sizeof(struct inode));
	inode->i_number = 0;
	inode->i_mtime = time(NULL);
	inode->i_type = 0;
	inode->i_size = 0;
	inode->i_blocks = 1;
	inode->direct_blk[0] = 0;
	inode->direct_blk[1] = -1;
	inode->indirect_blk = -1;
	inode->file_num = 0; //for now its 0
	//write
	lseek(fd, INODE_OFFSET, SEEK_SET); //point to INODE_OFFSET
	write(fd, (void *)inode, sizeof(struct inode));
	
	//each file in a directory have 1 dir_mapping in directory's datablock region 
	//each dir contains at least 2 dir_mapping: itself and parent
	//create rootDir itself dir_mapping
	rootDir = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
	strcpy(rootDir->dir,".");
	rootDir->inode_number=0;
	lseek(fd, DATA_OFFSET, SEEK_SET);
	write(fd, (void *)rootDir, sizeof(struct dir_mapping));	

	//update parameter in superblock zz
	sb = (struct superblock*)malloc(sizeof(struct superblock));
	sb->inode_offset = INODE_OFFSET;
	sb->data_offset = DATA_OFFSET;
	sb->max_inode = MAX_INODE;
	sb->max_data_blk = MAX_DATA_BLK;
	sb->blk_size = BLOCK_SIZE;
	sb->next_available_inode = 1;
	sb->next_available_blk = 1;
	//write
	lseek(fd, SB_OFFSET, SEEK_SET); //point to SB_OFFSET = 512B
	write(fd, (void *)sb, sizeof(struct superblock));

	//read to test
	// struct superblock x;
	// struct inode y;
	// struct dir_mapping z;

	// lseek(fd, SB_OFFSET, SEEK_SET);
	// read(fd, (void *)&x, sizeof(struct superblock));
	// printf("sb nextINODE %d;\n", x.next_available_inode); //1

	// lseek(fd, INODE_OFFSET, SEEK_SET); 
	// read(fd, (void *)&y, sizeof(struct inode));
	// printf("inode of root: direct_blk[0] %d;\n", y.direct_blk[0]); //0

	// lseek(fd, DATA_OFFSET, SEEK_SET);
	// read(fd, (void *)&z, sizeof(struct dir_mapping));
	// printf("inode_number of root %d;\n", z.inode_number); //0
	printf("File system established.\n");
	return 0;
}