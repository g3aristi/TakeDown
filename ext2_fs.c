#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext2_fs.h"

#define BS 1024 /*BLOCK SIZE*/

FILE *file;
struct super_block sb;
struct group_desc gd;
char inode_bitmap[16];
char data_bitmap[16];
struct inode in;
struct dir_entry de;
char *path;


int traverse(int prev_sde, char* token){
    printf("prev size of dir_entry: %d \n", prev_sde);
    if(prev_sde >= BS){
        return 1;
    } else {
        printf("------------------------------- RECURVIVE to the end ------------------ \n");
        fseek(file, BS*7+prev_sde, SEEK_SET);
        /*MAKE SURE TO CHANGE THE SIZE OF THE BYTES READ*/
        fread(&de, 1, sizeof(struct dir_entry), file);
        printf(" inode data located: %d \n", de.inode);
        printf(" dir_entry length: %d \n", de.rec_len);
        printf(" type : %d \n", de.file_type);
        printf(" actual name: %s \n", de.name);
        printf("prev size of dir_entry: %d \n", prev_sde);
        if(token == de.name){
            printf("Found token: %s in file %s \n", token, de.name);
        }
        prev_sde += de.rec_len;
        return traverse(prev_sde);
    }
}

char* strip_path(char *path){
    printf("stripping path: %s \n", path);
    char str[20] = "/lost+found/testfile.txt";
    char s[2] = "/";
    char *token;
   
   /* get the first token */
   token = strtok(str, s);
   
   /* walk through other tokens */
   while( token != NULL ) 
   {
      printf( " %s\n", token );
      /* stripped the path, now i need to see if the current token matches 
      a file name in all dir_entries */
    
      token = strtok(NULL, s);
   }

    return token;
}

int main(int argc, char *argv[]){

    file = fopen("onefile.img", "r");
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
    /*since we know from super block that there are 16 inodes*/
    printf("Inode data block first pointer to a block %d \n", in.i_block[0]); /* Pointers to blocks */
    printf("Inode data block ptr 2 %d \n", in.i_block[1]);
    printf("Inode data block ptr 3 %d \n", in.i_block[2]);
    printf("Inode data block ptr 4 %d \n", in.i_block[3]);
    printf("Inode data block ptr 5  %d \n", in.i_block[4]);
    printf("Inode data block ptr 6 %d \n", in.i_block[5]);
    printf("Inode data block ptr 7 %d \n", in.i_block[6]);
    printf("Inode data block ptr 8 %d \n", in.i_block[7]);
    printf("Inode data block ptr 9 %d \n", in.i_block[8]);
    printf("Inode data block ptr 10 %d \n", in.i_block[9]);
    printf("Inode data block ptr 11 %d \n", in.i_block[10]);
    printf("Inode data block ptr 12 %d \n", in.i_block[11]);
    printf("Inode data block ptr 13 %d \n", in.i_block[12]);
    printf("Inode data block ptr 14 %d \n", in.i_block[13]);
    printf("Inode data block ptr 15 %d \n", in.i_block[14]);
    printf("Inode data block ptr 16 %d \n", in.i_block[15]);
    printf("-------------------------Inode data block ptr 17 %d \n", in.i_block[16]);

    printf("------------------------------- ROOT ------------------ \n");
    /*Reconstructing root*/
    fseek(file, BS*7, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), file);
    printf("read the dir_entry for root \n");
    printf("roots inode data located: %d \n", de.inode);
    printf("roots dir_entry length: %d \n", de.rec_len);
    printf("roots type : %d \n", de.file_type);
    printf("roots actual name: %s \n", de.name);

    printf("------------------------------- NEXT ENTRY AFTER ROOT ------------------ \n");
    /*get the size of the previous directory entry*/
    int prev_sde = de.rec_len;
    fseek(file, (BS*7)+prev_sde, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), file);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("------------------------------- NEXT ENTRY AFTER 2 ENTRY------------------ \n");
    /*get the size of the previous directory entry*/
    int prev_sde2 = prev_sde+de.rec_len;
    printf("size of fist entry + second entry %d \n", prev_sde2);
    fseek(file, (BS*7)+prev_sde2, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), file);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("------------------------------- NEXT ENTRY AFTER 3 ENTRY------------------ \n");
    /*get the size of the previous directory entry*/
    int prev_sde3 = prev_sde2+de.rec_len;
    fseek(file, (BS*7)+prev_sde3, SEEK_SET);
    fread(&de, 1, sizeof(struct dir_entry), file);
    printf(" inode data located: %d \n", de.inode);
    printf(" dir_entry length: %d \n", de.rec_len);
    printf(" type : %d \n", de.file_type);
    printf(" actual name: %s \n", de.name);

    printf("---------------------CALL TO TRAVERSING \n");
    int rec;
    rec = traverse(0);
    printf("%d \n", rec);

    printf("---------------------CALLING STRTOK \n");
    int min;
    min = min(3, 4);
    printf("testing for min func %d \n", min)
    char *path = "/lost+found/testfile.txt";
    strip_path(path);


    return(0);
}