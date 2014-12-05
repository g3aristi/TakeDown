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
#include <sys/stat.h>
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
    fseek(img, BLOCK_SIZE*LOC_I_BITMAP, SEEK_SET);
    fread(&ibm, 1, sizeof(struct inode_bitmap), img);
    print_ibm(ibm);
    int free_inode;
    free_inode = find_free_inode(ibm);
    printf(" ####################  index of the free inode %d \n", free_inode);

     /* find a free data block form the data bitmap*/
    fseek(img, BLOCK_SIZE*LOC_D_BITMAP, SEEK_SET);
    fread(&dbm, 1, sizeof(struct data_bitmap), img);
    print_dbm(dbm);
    int fdi;
    fdi = free_data_inode(dbm);
    printf("^^^^^^^^^^^^^^^^^^^^^ free data inode %d \n", fdi);

     /* now that i have the data block where this dir starts 
     reading its dir_entries and file space where i can write 
     the src file, check all dir_entries */

     /* first fine the size of the file, so we know when we are done reading */
    char *src_path;
    src_path = argv[2];
    struct stat st;
    int size;


    stat(src_path, &st);
    size = st.st_size;
    printf("src file size is using stat %d \n", size);

    char *name;
    name = strip_name(argv[2]);
    printf("this is the file name that we are trying to copy %s \n", name);


    /* make a struct dir_entry */
    struct dir_entry new_dir_entry;
    new_dir_entry.inode = free_inode;
    new_dir_entry.rec_len = 8+strlen(name);
    new_dir_entry.name_len = strlen(name);
    new_dir_entry.file_type = 1; /* IT should always be a file */ 
    strcpy(new_dir_entry.name, name);

    printf("$$$$$$$$$$$$ TESTING THE NEW DIR ENTRY $$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    printf(" inode data located: %d \n", new_dir_entry.inode);
    printf(" dir_entry length: %d \n", new_dir_entry.rec_len);
    printf(" type : %d \n", new_dir_entry.file_type);
    printf(" actual name: %s \n", new_dir_entry.name);

    /* find the data block of the dir inode of the destination dir */
    int data_block;
    data_block = get_data_from_inode(argv[1], last_dir);

    /* write the new dir_entry into the destination dir in one of its dir_entries */
    fseek(img, BLOCK_SIZE*data_block, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), img);
    int wrote_dir;
    wrote_dir = add_free_entry(argv[1], data_block, new_dir_entry);
    printf("------ wrote the dir entry %d ------- \n", wrote_dir);

    /* with the free inode link it to the free data block */

    /* write the content of src to the disck image */

    /* NOE REALLY CHANGE THE BITMAPS update the inode bitmap and the data bitmap */
    return(0);
}