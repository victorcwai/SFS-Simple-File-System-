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

	if (string != NULL) 
	{
		while ((token = strsep(&string, "/")) != NULL)
		{
			if(count>=1) //after root dir, start putting in dir/file name in array
			{
				strcpy(tokenArr[distance++],token);
			}
			count++;
		}

		count = 0;

		while(distance--)
		{
			//search through the data block(which contains dir_mapping)
			//of the dir(root first) to get dir_mapping of next dir/file
			struct dir_mapping mapping;
			
			if(distance==0 && (flags==0 || flags==1)) //reach the end of path, create new file
			{
				inodeNum = createFile(inodeNum,flags);
				return inodeNum;
			}

			for(int i=0; i<MAX_INODE; i++)
			{
				//go to the datablock that belongs to "inodeNum", 
				//then start searching it MAX_INODE times
				lseek(fd, DATA_OFFSET + inodeNum*BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
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

int createFile(int parentInodeNum, int flags)
{
	if(flags == 0) //create file
	{
		;
	}
	else //create dir
	{
		;
	}
}

struct superblock getSuperBlock() //just get struct, get next_available_inode/blk in open_t
{
	;
}

struct inode createInode(int inodeNum, int blkNum, int flags)
{
	;
}

struct dir_mapping createMapping(int parentInodeNum)
{
	;
}

struct dir_mapping createMapping_Dir(int inodeNum)
{
	;
}

int main()
{
	open_t("/1/2/3/4/5/6/7/8/9/10/file.f",0);
	return 0;
}
//dir_mapping = 16byte
//everytime u create file: 
//	update sb: get sb -> get next_available_inode/blk + update it
//	create inode: put next_available_inode/blk in inode -> create it -> put it inside using next_available_inode
//	create dir_mapping in parent dir: create dir_mapping -> put it inside using parent inode (for accessing parent's data blk)
//	(for dir file: create dir_mapping): create dir_mapping -> put it inside using next_available_inode (for accessing it's data blk)