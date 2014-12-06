/*
ext2_ln: This program takes three command line arguments. The first is the name of an ext2 formatted virtual disk. 
The other two are absolute paths on your ext2 formatted disk. The program should work like ln, creating a link 
from the first specified file to the second specified path. If the first location does not exist, if the second 
location already exists, or if the first location refers to a directory, then your program should return the 
appropriate error. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext2_fs.h"


FILE *img;
char *token;
char *token2;
char *des_path;
char *src_path;
char s[2] = "/";
int next_block;

struct dir_entry de;
struct inode in;

int matched_inode; /* inode to point to by the src */


/*First arg: The first is the name of an ext2 formatted virtual disk.
	Second arg:  src file 
	Thrid arg: dsetination file */


/*i.e creat a new dir_entry in the srcs dir_entries that points 
	to the destinations inode*/
int main(int argc, char *argv[]){

	/* open the img */
	if((img = fopen(argv[1], "r")) == NULL){
    	perror("Error opening the image: \n");
    	exit(1);
    }

    src_path = argv[2];
    des_path = argv[3];

	/*  traverse to destination trying to get the inode of the src*/
	token = strtok(des_path, s); 
   	int i= 0;
   	while(token != NULL) {
   		/* go to block and read all its dir_entries */
   		int des; /*directory entry size */
   		des = 0;

   		/* check all the blocks roots dir entry*/
   		while(des < 1024){
   			fseek(img, BLOCK_SIZE*7+des, SEEK_SET);
   			fread(&de, 1, sizeof(struct dir_entry), img);
   			printf(" inode data located: %d \n", de.inode);
		    printf(" dir_entry length: %d \n", de.rec_len);
		    printf(" type : %d \n", de.file_type);
		    printf(" actual name: %s \n", de.name);
		    int result;
		    result = strncmp(de.name, token, strlen(token));
		    printf("this is the result of srtncmp %d \n", result);



   			if(  (strncmp(de.name, token, strlen(token))   == 0 )){
   				/* go to inode to get to the next block */
   				matched_inode = de.inode;
   				fseek(img, BLOCK_SIZE*5+128*de.inode, SEEK_SET);
   				fread(&in, 1, sizeof(struct dir_entry), img);
   				int j;

   				/* check all the inodes block pointer to see where to go next*/
   				for(j = 0; j < 15; j++){
   					if(in.i_block[j] != 0){
   						next_block = in.i_block[j];
   						printf("this the next block to go to %d \n", next_block);
   						break;
   					}
   				}

   				break;
   			}
   			des += de.rec_len;
   		}
   		token = strtok(NULL, s);
    }

	/*  get the inode of destination */
	printf("this is the inode needed to link %d \n", matched_inode);

	/*  traverse to src  */
	token2 = strtok(src_path, s);
	while(token != NULL) {
		/* check direntires of root and find the first token */
		/* the dir_entry.name that matches the token holds the inode index 
			in the inode table where the next token should be */
		/* go to the inode table and find where the data block is for this inode */


		/* read the dir_entries of the new directory and check for the next token */
		/* where the dir_entry.name matches the second token hold the inode index 
			in the inode talbe where the next dir is */
		/* go to index in the inode talbe 1024*3+128*inode index to find where the next
			data block should go next */

		/* recursively repeat until we reach the end of the path */
		/* at the end of the path store the srcs block # where its 
			directories entries start */
   		token2 = strtok(NULL, s);
    }

	/* now that we have found the inode location in the inode table of the destination
		and we have the block number where the srcs dir_entries starts, we can split */

    /* split a dir_entry that has beeind padded with the size of the block size 
    	we can find out the dirs actual actual size in the folling matter :
		
		actual dir_entry size is dir_entry.name_len + 8

		then we can subtract the entries previous rec_len - its actual dir_entry size
		to check if there is enough space after splitting and 
		write the new dir_entry after this current dir_entry

    	*/

    /* the new dir_entry should contain:

    	new_dir_entry->inode = src's inode ;
	    new_dir_entry->rec_len = 8+strlen(name of src file, i.e lats token read);
	    new_dir_entry->name_len = strlen(name);
	    new_dir_entry->file_type = 1;

    */

	return 0;
}