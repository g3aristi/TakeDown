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
    struct inode_bitmap ibm;
    fseek(img, BLOCK_SIZE*4, SEEK_SET);
    fread(&ibm, 1, sizeof(struct inode_bitmap), img);
    print_ibm(ibm);
    //inode_bm = printb(argv[1], LEN_INODE_BITMAP, LOC_I_BITMAP);
    //printf("+++++++++++++++++bitmap : %u \n", inode_bm.inodes);

     /* find a free data block form the data bitmap*/
    struct data_bitmap dbm;
    fseek(img, BLOCK_SIZE*3, SEEK_SET);
    fread(&dbm, 1, sizeof(struct data_bitmap), img);
    print_dbm(dbm);


     /*now tha i have the data block where this dir starts 
     reading its dir_entries and file space where i can write 
     the src file, check all dir_entries */

     /* make a struct dir_entry */

     /*write the content of src to the disck image */

     /* NOE REALLY CHANGE THE BITMAPS update the inode bitmap and the data bitmap */








    //printf("striped path token received %s \n", tk1);

    // int found;
    // found = find_file(argv[1], tk1, 7);
    // printf("found %d \n", found);

    // char *tk2;
    // tk2 = strip_path(path);



    /*go to lockation of root */
    printf("------------------------------- ROOT ------------------ \n");
    fseek(img, BLOCK_SIZE*7, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);


    printf("------------------------------- NEXT BLOCK ------------------ \n");
    fseek(img, BLOCK_SIZE*7+12, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);


    printf("------------------------------- NEXT BLOCK ------------------ \n");
    fseek(img, BLOCK_SIZE*7+12+12, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("------------------------------- NEXT BLOCK ------------------ \n");
    fseek(img, BLOCK_SIZE*7+12+12+20, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);


    /* root told me that the inode for the testdirectory is at inode #12*/
    printf("------------------------------- INODE #11 ------------------ \n");
    fseek(img, BLOCK_SIZE*5+128*11, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), img);
    printf(" inode data located: %d \n", in.i_block[0]);

    /* the inode tells me where the data for testdir is, block # 21*/
    printf("------------------------------- BLOCK #127 ------------------ \n");
    printf("-------------------------------FIRST DIR_ENTRY \n");
    fseek(img, BLOCK_SIZE*127, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("------------------------------- SECOND DIR_ENTRY \n");
    fseek(img, BLOCK_SIZE*127+12, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("------------------------------- THIRD DIR_ENTRY \n");
    fseek(img, BLOCK_SIZE*127+12+12, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("*************************************THIS IS THE ACTUAL NODE %d \n", de.inode);



    printf("size of inode struct %lu \n", sizeof(struct inode_bitmap));
    printf("size of data struct %lu \n", sizeof(struct data_bitmap));


    return(0);
}