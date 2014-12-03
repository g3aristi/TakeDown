#include <stdio.h>
#include <stdlib.h>
#include "ext2_fs.h"

#define BS 1024 /*BLOCK SIZE*/

FILE *file;
struct super_block sb;
struct group_desc gd;
char inode_bitmap[16];
char data_bitmap[16];
struct inode in;
struct dir_entry de;

int main(int argc, char *argv[]){

    file = fopen("emptydisk.img", "r");
    /*reconstructiong the super block*/
    fseek(file, BS, SEEK_SET);
    fread(&sb, 1, sizeof(struct super_block), file);
    printf("------------------------------- SUPER BLOCK----------------------- \n");
    printf(" Inodes count %d \n", sb.s_inodes_count);
    printf(" Blocks count %d \n", sb.s_blocks_count);
    printf(" Free blocks count %d \n", sb.s_free_blocks_count);
    printf(" Free inodes count %d \n", sb.s_free_inodes_count);

    /* reconstructing the group descriptor, which is at block #2 */
    fseek(file, BS*2, SEEK_SET);
    fread(&gd, 1, sizeof(struct group_desc), file);
    printf("------------------------------- GROUP DESCRIPTOR------------------ \n");
    printf("Blocks bitmap block  %d \n", gd.bg_block_bitmap);
    printf("Inodes bitmap block  %d \n", gd.bg_inode_bitmap);
    printf("Inodes table block   %d \n", gd.bg_inode_table);
    printf("Free blocks count  %d \n", gd.bg_free_blocks_count);
    printf("Free inodes count  %d \n", gd.bg_free_inodes_count);
    printf("Directories count %d \n", gd.bg_used_dirs_count);

    /* reconstructing the Inode bitmap, which is at block #4*/
    printf("------------------------------- INODE BITMAP ------------------ \n");
    fseek(file, BS*4, SEEK_SET);
    fread(&inode_bitmap, 1, sizeof(char)*16, file);
    printf("inode bitmap: %s \n", inode_bitmap);

    /* reconstructing the data bitmap, which is at block #3*/
    printf("------------------------------- DATA BITMAP ------------------ \n");
    fseek(file, BS*3, SEEK_SET);
    fread(&data_bitmap, 1, sizeof(char)*16, file);
    printf(" data bitmap: %s \n", data_bitmap);

    printf("------------------------------- INODE ------------------ \n");
    /* Rescontruct the inode table, which is at block#5. This will lead me to root */
    fseek(file, BS*5 + 128, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), file);
    printf("Inode data block %d \n", in.i_block[0]); /* Pointers to blocks */
    /*Reconstructing root*/

    return(0);
}