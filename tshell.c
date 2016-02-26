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
#include "open_t.h"
#include "read_t.h"
#include "write_t.h"
int main()
{
	// open_t("/sad",1);
	// open_t("/lemon",0);
	// open_t("/lemon",0);
	// open_t("/sad/abcd",1);
	printf("tshell###--You are in root directory now.\n$");
	char* str = NULL;
	char command[10];
	char* token = NULL;
	char parameter[2][50];
	parameter[0][0] = '\0';
	parameter[1][0] = '\0';
	size_t len = MAX_COMMAND_LENGTH;
	int currentDirInumber = 0; //root
	struct inode inode = getInode(0); //root
	struct inode tempInode;
	int childpid;
	int fd = open ("HD", O_RDWR, 660);
	int i; //loop count
	while(1){  //loop user input, fork -> wait child finish -> loop
		while(1){
			int result = getline(&str, &len, stdin);
			if (result == -1) 
			{
				printf("getline() error. Exit.\n");
				return -1;
			}else if(str[0] == '\n')
			{
				command[0] = '\0';
			}else
			{
				str[result-1] = '\0'; //remove \n
				token = strtok(str, " ");
				strcpy(command, token);
			}
//command received------------------------------------------------------------
			if(strcmp(command, "ls_t") == 0){
				if ((childpid = fork()) == 0) 
				{
					//child
					struct dir_mapping mapping;
					printf("inode number	file type 	size 		   	   name			      creation time\n");
					int fileNum = 2;
					//update inode status!
					inode = getInode(inode.i_number);
					if (inode.i_number == 0) fileNum = 0; //if it is root directory
					for(i=0; i<inode.file_num+fileNum; i++) //+2 for parent and self directory
					{
						lseek(fd, DATA_OFFSET + inode.direct_blk[0] * BLOCK_SIZE + i*sizeof(struct dir_mapping), SEEK_SET); //asd
						//lseek(fd, DATA_OFFSET, SEEK_SET);
						read(fd, &mapping, sizeof(struct dir_mapping));					
						tempInode = getInode(mapping.inode_number);
						printf("%d 	%9d 	  %7d		%15s %36s\n",mapping.inode_number,tempInode.i_type,tempInode.i_size,mapping.dir, ctime(&tempInode.i_mtime));
					}
					exit(0);
				} else if (childpid > 0) 
				{
					//parent
					wait();
				}
			}else if(strcmp(command, "cd_t") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Incorrect parameter. Try again please.\n");
					break;
				}
				strcpy(parameter[0], token);
				if(strcmp(parameter[0],"root")==0)
				{
					printf("Current directory is root\n");
					currentDirInumber = 0;
					inode = getInode(0);
					break;
				}
				int temp = open_t(parameter[0],2);
				if(temp == -1)
				{
					printf("Can't find path. Please try again.\n");
				}else
				{
					if(getInode(temp).i_type == 1)
					{
						printf("This is a file, not a directory. Current directory does not change. Try again please.\n");
						break;
					}
					printf("Current directory is: %s\n",parameter[0]);
					currentDirInumber = temp;
					inode = getInode(temp);
				}

			}else if(strcmp(command, "mkdir_t") == 0){
				token = strtok(NULL, " ");
				strcpy(parameter[0], token);
				if(token==NULL)
				{
					printf("Incorrect parameter.\n");
					break;
				}
				if ((childpid = fork()) == 0) 
				{
					//child
					struct superblock sb;
					sb = getSuperBlock();
					int next_available_inode = sb.next_available_inode;
					int next_available_blk = sb.next_available_blk;
					struct dir_mapping mapping;
					for(i=0; i<inode.file_num+2; i++)
					{
						lseek(fd, DATA_OFFSET + inode.direct_blk[0] * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
						read(fd, (void *)&mapping, sizeof(struct dir_mapping));
						if(strcmp(parameter[0],mapping.dir)==0) //there exist a file with the same name
						{
							printf("A directory with the same name exists. The new directory will replace it.\n");
							int newInode = mapping.inode_number;
							replaceFile(mapping.inode_number,1);
							exit(0);
						}
					}
					printf("Directory created, inode is: %d\n", createInode(parameter[0], inode, currentDirInumber, next_available_inode, next_available_blk, 1));
					//update sb
					// sb.inode_offset = INODE_OFFSET;
					// sb.data_offset = DATA_OFFSET;
					// sb.max_inode = MAX_INODE;
					// sb.max_data_blk = MAX_DATA_BLK;
					// sb.blk_size = BLOCK_SIZE;
					// sb.next_available_inode = (next_available_inode+1);
					// sb.next_available_blk = (next_available_blk+1);
					// lseek(fd, SB_OFFSET, SEEK_SET);
					// write(fd, &sb, sizeof(struct superblock));

					exit(0);
				} else if (childpid > 0) 
				{
					//parent
					wait();
				}
			}else if(strcmp(command, "external_cp") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Incorrect parameter.\n");
					break;
				}
				strcpy(parameter[0], token);
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Incorrect parameter.\n");
					break;
				}
				strcpy(parameter[1], token);
				if ((childpid = fork()) == 0) 
				{
					//child
					//read file by byte
					FILE *f = fopen(parameter[0], "rb");
					
					if(f)
					{
						fseek(f, 0, SEEK_END);
						long fsize = ftell(f);
						fseek(f, 0, SEEK_SET);
						char *string = malloc(fsize + 1);
						fread(string, fsize, 1, f);
						fclose(f);
						string[fsize] = 0;
						int i_number = open_t(parameter[1],0);
						if(i_number == -1)
						{
							printf("File not found. Try again please.\n");
							exit(0);
						}
						write_t(i_number,0,string,fsize);
						printf("Write successful.\n");
						exit(0);
					}else
					{
						printf("No external file is found. Try again please.\n");
						exit(0);
					}
				} else if (childpid > 0) 
				{
					//parent
					wait();
				}

			}else if(strcmp(command, "cp_t") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Incorrect parameter.\n");
					break;
				}
				strcpy(parameter[0], token);
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Incorrect parameter.\n");
					break;
				}
				strcpy(parameter[1], token);
				if ((childpid = fork()) == 0) 
				{
					//child
					int i_number1 = open_t(parameter[0],2);
					struct inode inode1 = getInode(i_number1);
					char * string = malloc(inode1.i_size);
					if(i_number1 == -1)
					{
						printf("File to read not found. Try again please.\n");
						exit(0);
					}
					read_t(i_number1,0,string,inode1.i_size);

					int i_number2 = open_t(parameter[1],0);
					struct inode inode2 = getInode(i_number2);
					if(i_number2 == -1)
					{
						printf("File to write not found. Try again please.\n");
						exit(0);
					}
					write_t(i_number2,0,string,inode1.i_size);
					printf("Write successful.\n");
					exit(0);
				} else if (childpid > 0) 
				{
					//parent
					wait();
				}
			}else if(strcmp(command, "cat_t") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Incorrect parameter.\n");
					break;
				}
				strcpy(parameter[0], token);
				if ((childpid = fork()) == 0) 
				{
					//child
					int i_number = open_t(parameter[0],2);
					if(i_number == -1)
					{
						printf("File not found. Try again please.\n");
						exit(0);
					}
					struct inode tempInode = getInode(i_number);			
					char* buf = malloc(tempInode.i_size);
					read_t(tempInode.i_number,0,buf,tempInode.i_size);
					printf("%s",buf);
					exit(0);
				} else if (childpid > 0) 
				{
					//parent
					wait();
				}
			}else if(strcmp(command, "exit") == 0){
				printf("Shell exited.\n");
				return 0;
			}else{
				printf("Unknown command %s.",command);
			}
			printf("\n$");
		}
		printf("\n$");
	}
}
