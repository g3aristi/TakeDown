/*
ext2_cp: This program takes three command line arguments. The first is the name of an ext2 formatted virtual disk. 
The second is the path to a file on your native operating system, and the third is an absolute path on your ext2 
formatted disk. The program should work like cp, copying the file on your native file system onto the specified 
location on the disk. If the specified file or target location does not exist, then your program should 
return the appropriate error. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext2_fs.h"

FILE *img;
FILE *src;
char *path;
struct dir_entry de;
struct inode in;

struct inode_bitmap ibm;
struct data_bitmap dbm;


/*First arg is the image, second the src file, third is the destination*/
int main(int argc, char *argv[]){
    if((img = fopen(argv[1], "r")) == NULL){
    	perror("Error opening the image: \n");
    	exit(1);
    }

    if((src = fopen(argv[2], "r")) == NULL){
    	perror("Error opening the source file: \n");
    	exit(1);
    }

    path = argv[3];

    int last_dir;
    last_dir = strip_path(argv[1], path);
    printf("location of the path files node is at: %d \n", last_dir);

     /*find a free inode */
    fseek(img, BLOCK_SIZE*4, SEEK_SET);
    fread(&ibm, 1, sizeof(struct inode_bitmap), img);
    print_ibm(ibm);
    int free_inode;
    free_inode = find_free_inode(ibm);
    printf(" ______________index of the inode %d \n", free_inode);

     /* find a free data block form the data bitmap*/
    fseek(img, BLOCK_SIZE*3, SEEK_SET);
    fread(&dbm, 1, sizeof(struct data_bitmap), img);
    print_dbm(dbm);

     /*now tha i have the data block where this dir starts 
     reading its dir_entries and file space where i can write 
     the src file, check all dir_entries */

     /* make a struct dir_entry */

     /*write the content of src to the disck image */

     /* NOE REALLY CHANGE THE BITMAPS update the inode bitmap and the data bitmap */
    return(0);
}