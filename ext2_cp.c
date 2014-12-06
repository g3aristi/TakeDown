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
    int fdb;
    fdb = free_data_inode(dbm) + 1; /* plus one since data blocks start at 1 */
    printf("^^^^^^^^^^^^^^^^^^^^^ free data block %d \n", fdb); 

     /* now that i have the data block where this dir starts 
     reading its dir_entries and file space where i can write 
     the src file, check all dir_entries */

     /* first fine the size of the file, so we know when we are done reading */
    char *src_path;
    src_path = argv[2];
    struct stat st;
    int file_size;

    stat(src_path, &st);
    file_size = st.st_size;
    char *name;
    name = strip_name(argv[2]);


    /* make a struct dir_entry */
    struct dir_entry *new_dir_entry = malloc(sizeof(struct dir_entry));
    new_dir_entry->inode = free_inode;
    new_dir_entry->rec_len = 8+strlen(name);
    new_dir_entry->name_len = strlen(name);
    new_dir_entry->file_type = 1; /* IT should always be a file */ 
    strcpy(new_dir_entry->name, name);


    /* find the data block of the dir inode of the destination dir */
    int data_block;
    data_block = get_data_from_inode(argv[1], last_dir);
    printf("this is the data_block passed in #: %d \n", data_block);

    /* write the new dir_entry into the destination dir in one of its dir_entries */
    fseek(img, BLOCK_SIZE*data_block, SEEK_SET);


    fread(&de, 1, sizeof(struct dir_entry), img);

    add_free_entry(argv[1], data_block, new_dir_entry);
    //printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");


    // /* with the free inode link it to the free data block */

    fseek(img, BLOCK_SIZE*LOC_I_TABLE+INODE_SIZE*free_inode, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), img);
    in.i_mode = 0x8777; /* this for a file */
    in.i_size = 1024;
    in.i_links_count = 0; 
    in.i_uid = 0; /* Low 16 bits of Owner Uid */ 
    in.i_atime = 0; /* Access time */ 
    in.i_ctime = 0; /* Inode change time */ 
    in.i_mtime = 0; /* Modification time */ 
    in.i_dtime = 0; /* Deletion Time */ 
    in.i_gid = 0; /* Low 16 bits of Group Id */ 
    in.i_blocks = 6; /* Blocks count */ 
    in.i_flags = 0; /* File flags */

    /* write the content of src file to the disck image */
    int wrote;

    /* NOE REALLY CHANGE THE BITMAPS update the inode bitmap and the data bitmap */
    wrote = write_data(argv[1], argv[2], file_size, fdb, in, dbm, ibm, free_inode);
    printf("did i write to the file????: %d \n", wrote);


    /* I node that at some point i need to update 
        the super block with all the changes */

    /* I tryied creating dir_entries but i didnt know how to fully populate them*/

    /* i did change the bit maps, but i did it wrong and 
        it broke the rest of the functions*/

    //print_dbm(dbm);
    return(0);
}