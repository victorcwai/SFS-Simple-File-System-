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
struct superblock getSuperBlock();
struct inode getInode(int inode_number);
int createInode(char* name, struct inode parentInode, int parentInodeNum, int next_available_inode, int next_available_blk, int flags);
void createMapping(char* name, int inodeNum, struct inode parentInode, int parentInodeNum);



int open_t( const char *pathname, int flags)
{
	char* token;
	char* string;
	string = strdup(pathname);
	int found = 0;
	int inodeNum = 0; //root dir's inode number
	int distance = 0;
	int count = 0;
	char tokenArr[11][509];
	char* dirList[10];
	int fd = open ("HD", O_RDWR, 660);
	int dirDataBlkIndex;
	int i;
	if (string != NULL) 
	{
		while ((token = strsep(&string, "/")) != NULL) //split path name
		{
			if(count>=1) //after root dir, start putting in dir/file name in array
			{
				strcpy(tokenArr[distance++],token);
				printf(token);
			}
			count++;
		}

		count = 0;

		while(distance--)
		{
			//search through the data block(which contains dir_mapping)
			//of the dir(root first) to get dir_mapping of next dir/file
			struct dir_mapping mapping;
			struct inode inode_;
			inode_ = getInode(inodeNum); //inode of parent dir

			dirDataBlkIndex = inode_.direct_blk[0];

			if(distance==0 && (flags==0 || flags==1)) //reach the end of path, create new file (dont need to search!)
			{
				close(fd);
				struct superblock sb;
				sb = getSuperBlock();
				int next_available_inode = sb.next_available_inode;
				int next_available_blk = sb.next_available_blk;
				inodeNum = createInode(tokenArr[count], inode_, inodeNum, next_available_inode, next_available_blk, flags);

				//update sb
				return inodeNum;
			}

			for(i=0; i<MAX_INODE; i++)
			{
				//go to the datablock that belongs to "inodeNum", 
				//then start searching it MAX_INODE times
				lseek(fd, DATA_OFFSET + dirDataBlkIndex * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
				read(fd, (void *)&mapping, sizeof(struct dir_mapping));
				//got the mapping, see if the next dir/file exist
				if(strcmp(tokenArr[count],mapping.dir)==0)
				{
					found = 1;
					inodeNum = mapping.inode_number;
					printf("matching name is:  %d;\n and it's inode num is:  %d;", mapping.dir, mapping.inode_number);
					if(distance==0 && flags==2) //reach the end of path and flag is 2
						return inodeNum;
					break;
				}
			} 

			if(found==0)
			{
				printf("Error 404: File not found.\n");
				return -1;
			}
			else
			{
				count++;
				found = 0; //move on to next dir/file
			}
		}
	}

	if(found==0)
	{
		printf("Error 404: File not found.\n");
		return -1;
	}
	else return inodeNum; //last inodeNum found is also the answer
}

struct superblock getSuperBlock() //just get struct, get next_available_inode/blk in open_t
{
	int fd = open ("HD", O_RDWR, 660);
	struct superblock sb;
	lseek(fd, SB_OFFSET, SEEK_SET);
	read(fd, (void *)&sb, sizeof(struct superblock));
	printf("sb nextINODE %d;\n", sb.next_available_inode);
	close(fd);
	return sb;
}

struct inode getInode(int inode_number)
{
	int fd = open ("HD", O_RDWR, 660);
	struct inode inode_;
	lseek(fd, INODE_OFFSET+inode_number*sizeof(struct inode), SEEK_SET); 
	read(fd, (void *)&inode_, sizeof(struct inode));
	printf("getInode.i_number %d;\n", inode_.i_number);
	close(fd);
	return inode_;
}

int createInode(char* name, struct inode parentInode, int parentInodeNum, int next_available_inode, int next_available_blk, int flags)
{
	int fd = open ("HD", O_RDWR, 660);
	int fileType;
	if(flags==0) fileType = 1;
	else if(flags==1) fileType = 0;

	struct inode* inode;
	inode = (struct inode*)malloc(sizeof(struct inode));
	inode->i_number = next_available_inode;
	inode->i_mtime = time(NULL);
	inode->i_type = fileType;
	inode->i_size = 0;
	inode->i_blocks = 1;
	inode->direct_blk[0] = next_available_blk;
	inode->direct_blk[1] = -1;
	inode->indirect_blk = -1;

	//create dir_mapping ".", ".." if the file is a directory:
	if(fileType==0)
	{
		inode->file_num = 2; //self+parent
		struct dir_mapping* self;
		self = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
		strcpy(self->dir,".");
		self->inode_number = next_available_inode;

		struct dir_mapping* parent;
		parent = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
		strcpy(parent->dir, "..");
		parent->inode_number = parentInodeNum;

		lseek(fd, DATA_OFFSET + inode->direct_blk[0]*BLOCK_SIZE + sizeof(struct dir_mapping), SEEK_SET);
		write(fd, (void *)self, sizeof(struct dir_mapping));

		lseek(fd, DATA_OFFSET + inode->direct_blk[0]*BLOCK_SIZE + sizeof(struct dir_mapping)*2, SEEK_SET);
		write(fd, (void *)parent, sizeof(struct dir_mapping));
	}
	else
	{
		inode->file_num = 0;
	}

	lseek(fd, INODE_OFFSET+sizeof(struct inode)*next_available_inode, SEEK_SET);
	write(fd, (void *)inode, sizeof(struct inode));

	//update superblock
	struct superblock* sb;
	sb = (struct superblock*)malloc(sizeof(struct superblock));
	sb->inode_offset = INODE_OFFSET;
	sb->data_offset = DATA_OFFSET;
	sb->max_inode = MAX_INODE;
	sb->max_data_blk = MAX_DATA_BLK;
	sb->blk_size = BLOCK_SIZE;
	sb->next_available_inode = (next_available_inode+1);
	sb->next_available_blk = (next_available_blk+1);

	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, (void *)sb, sizeof(struct superblock));
	close(fd);

	//create dir_mapping in parent dir
	createMapping(name,next_available_inode,parentInode,parentInodeNum);

	return next_available_inode;
}

void createMapping(char* name, int inodeNum, struct inode parentInode, int parentInodeNum)
{
	int fd = open ("HD", O_RDWR, 660);
	struct dir_mapping* mapping;
	mapping = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
	strcpy(mapping->dir, name);
	mapping->inode_number = inodeNum;
	lseek(fd, DATA_OFFSET + parentInode.direct_blk[0] * BLOCK_SIZE + (parentInode.file_num * sizeof(struct dir_mapping)), SEEK_SET);
	write(fd, (void *)mapping, sizeof(struct dir_mapping));
	//update parent dir inode
	struct inode* parent;
	parent->i_number = parentInode.i_number;
	parent->i_mtime = parentInode.i_mtime;
	parent->i_type = parentInode.i_type;
	parent->i_size = parentInode.i_size;
	parent->i_blocks = parentInode.i_blocks;
	parent->direct_blk[0] = parentInode.direct_blk[0];
	parent->direct_blk[1] = parentInode.direct_blk[1];
	parent->indirect_blk = parentInode.indirect_blk;
	parent->file_num = parentInode.file_num+1; //update here 

	lseek(fd, INODE_OFFSET+parentInodeNum*sizeof(struct inode), SEEK_SET); 
	write(fd, (void *)parent, sizeof(struct inode));
	close(fd);
}

int main()
{
	open_t("/",2);
	return 0;
}
//dir_mapping = 16byte
//everytime u create file: 
//	update sb: get sb -> get next_available_inode/blk + update it
//	create inode: put next_available_inode/blk in inode -> create it -> put it inside using next_available_inode
//	create dir_mapping in parent dir: create dir_mapping -> put it inside using parent inode (for accessing parent's data blk) -> update parent inode dir's file_num
//	(for dir file: create dir_mapping): create dir_mapping -> put it inside using next_available_inode (for accessing it's data blk)