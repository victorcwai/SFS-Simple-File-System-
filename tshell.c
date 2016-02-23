#include <stdio.h>
#include <sfsheader.h>

int main(){
	printf("tshell###\n$");
	char str[100];
	while(1){  //loop user input, fork -> wait child finish -> loop
		scanf("%s",str);

		if(strcmp(str, "ls_t") == 0){
			
		}else if(strcmp(str, "cd_t") == 0){
			
		}else if(strcmp(str, "mkdir_t") == 0){
			
		}else if(strcmp(str, "external_cp") == 0){
			
		}else if(strcmp(str, "cp_t") == 0){
			
		}else if(strcmp(str, "cat_t") == 0){
			
		}else if(strcmp(str, "exit") == 0){
			return 0;
		}else{
			printf("unknown command %s",str);
		}
		printf("\n$");
	}
}
