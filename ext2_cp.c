/*
ext2_cp: This program takes three command line arguments. The first is the name of an ext2 formatted virtual disk. 
The second is the path to a file on your native operating system, and the third is an absolute path on your ext2 
formatted disk. The program should work like cp, copying the file on your native file system onto the specified 
location on the disk. If the specified file or target location does not exist, then your program should 
return the appropriate error. 
*/

#include <stdio.h>
#include <stdlib.h>

#define SBLOCATION
#define INODEMAP
#define DATAMAP

struct superBlock{
	char *content;
};

int main (int argc, char *argv[]){

	if(argc != 2){
		printf("invalid number of arguments \n");
	

	} else {
		FILE *file = fopen(argv[1], "r");
		if(file == 0){
			printf("Could not open file %s \n", argv[1]);
		
		} else {
			struct superBlock sb = malloc(sizeof(struct superBlock));
			fread(sb, SBLOCATION, 1, file);
			printf("$s \n", sb->content);
		}
	}	
}