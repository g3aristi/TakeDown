#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext2_fs.h"

#define BS 1024 /*BLOCK SIZE*/
#define FDEAR 24 /*First Directory Entry After Root*/

FILE *file;
struct dir_entry de;
struct inode in;


char inode_bitmap[16];
char data_bitmap[16];
char *path;



int get_data_from_inode(char *img, int inode_num){

    if((file = fopen(img, "r")) == NULL){
        perror("Error opening the image: \n");
        exit(1);
    }

    fseek(file, BLOCK_SIZE*LOC_I_TABLE+INODE_SIZE*inode_num, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), file);
    //printf(" inode data located: %d \n", in.i_block[0]);
    /* should loop through all the data block pointers */
    return in.i_block[0];

}


/* return the inode of the data bloock of the the mathching file */
int find_inode(char *img, char *token, int block_num){
    //block_num --;
    printf("----------------------current block %d \n", block_num);
    printf("block %d, has token %s \n", block_num, token);
    char tk_cp[strlen(token+1)];
    strcpy(tk_cp, token);

    if((file = fopen(img, "r")) == NULL){
        perror("Error opening the image: \n");
        exit(1);
    }

    int size;
    size = 0;

    /*create funtion that read all dir entries*/
    while(size < BLOCK_SIZE){
        fseek(file, BLOCK_SIZE*block_num+size, SEEK_SET);
        fread(&de, 1, sizeof(struct dir_entry), file);
        printf(" inode data located: %d \n", de.inode);
        printf(" dir_entry length: %d \n", de.rec_len);
        printf(" type : %d \n", de.file_type);
        printf(" actual name: %s \n", de.name);
        if((strncmp(tk_cp, de.name, strlen(token))) == 0){
            printf("FOUND THE TOKEN \n");
            break;
        }

        size += de.rec_len;

    }

    return de.inode;

}

/* returns the location of the data for this final path */
int strip_path(char *img, char *path){
    int path_len, loc_inode, next_inode, next_data_block, next_inode2;
    path_len = strlen(path);
    char path_cpy[path_len];
    strcpy(path_cpy, path);
    //printf("path copy: %s \n", path_cpy);
    char s[2] = "/";
    char *token;
   
   /* get the first token */
   token = strtok(path_cpy, s); 

   int i= 0;
   while(token != NULL) {
        printf("TOKEN %s \n", token);
        /*if is the first token, then i should start and the root*/
        if(i == 0){
            loc_inode = find_inode(img, token, LOC_ROOT);
            token = strtok(NULL, s);

        /*i should start when ever the inode lead me to*/
        } else {

            /*next_inode holds the inode to where i should go next */
            next_inode = loc_inode;
            next_data_block = get_data_from_inode(img, next_inode - 1);
            loc_inode = find_inode(img, token, next_data_block);
            token = strtok(NULL, s);
        }
        i++;
    }
    return loc_inode;
}


void print_ibm(struct inode_bitmap ibm){  
    // __u64 bitmap1; 
    // __u64 bitmap2;
    // fread(&bitmap1, 1, sizeof(__u64), fp); 
    // fread(&bitmap2, 1, sizeof(__u64), fp);
    printf("This is the inode bit map \n");
    int i;
    for(i = 0; i < 16; i++){ 
        if((i % 8) == 0) 
            printf(" "); 
        if (ibm.inodes & 1) 
            printf("1"); 
        else 
            printf("0"); 
    }
    ibm.inodes >>= 1; 
    printf("\n");
}

void print_dbm(struct data_bitmap dbm){  
    // __u64 bitmap1; 
    // __u64 bitmap2;
    // fread(&bitmap1, 1, sizeof(__u64), fp); 
    // fread(&bitmap2, 1, sizeof(__u64), fp);
    int i;
    printf("This is the data1 bit map \n");
    /* first 64 bits since the struct has that much space*/
    for(i = 0; i < 64; i++){ 
        if((i % 8) == 0) 
            printf(" "); 
        if (dbm.data1 & 1) 
            printf("1"); 
        else 
            printf("0"); 
    }
    dbm.data1 >>= 1; 
    printf("\n");

    /* the remaining 64 bits since the struct has that much space*/
    printf("This is the data2 bit map \n");
    int j;
    for(j = 64; j >= 128; i++){ 
        if((j % 8) == 0) 
            printf(" "); 
        if (dbm.data2 & 1) 
            printf("1"); 
        else 
            printf("0"); 
    }
    dbm.data2 >>= 1; 
    printf("\n");
}
//     if(n == 128){
//         for(i = 0; i < n; i++){ 
//             if((i % 8) == 0) 
//                 printf(" "); 
//             if (bitmap1 & 1) 
//                 printf("1"); 
//             else 
//                 printf("0"); 
//         bitmap1 >>= 1; 
//         }
//     } 
//     puts(""); 
//     if(n < 64) 
//         return;

//     for(i=0;i<n;i++){ 
//         if((i % 8) == 0) 
//             printf(" "); 
//         if (bitmap2 & 1) printf("1"); 
//         else printf("0"); bitmap2 >>= 1; 
//     } 
//     puts("");
//     puts("");

//     } 
// }

// int traverse(int prev_sde, char* token){
//     int len_token, len_de_name;

//     printf("prev size of dir_entry: %d \n", prev_sde);
//     if(prev_sde >= BS){
//         return 1;
//     } else {
//         printf("------------------------------- RECURVIVE to the end ------------------ \n");
//         fseek(file, BS*7+prev_sde, SEEK_SET);
//         /*MAKE SURE TO CHANGE THE SIZE OF THE BYTES READ*/
//         fread(&de, 1, sizeof(struct dir_entry), file);
//         printf(" inode data located: %d \n", de.inode);
//         printf(" dir_entry length: %d \n", de.rec_len);
//         printf(" type : %d \n", de.file_type);
//         printf(" actual name: %s \n", de.name);
//         //printf("prev size of dir_entry: %d \n", prev_sde);
//         /*If the names are not equal in length then move on!*/
//         printf("this is the token im looking for in the dir_entry %s \n", token);
//         printf("and current files name %s \n", de.name);
//         len_token = strlen(token);
//         len_de_name = strlen(de.name);
//         //printf("this is the token size %d \n", len_token);
//         //printf("and current files name size %d \n", len_de_name);
//         if(len_token != len_de_name){
//             printf("the file name was not found in this dir_entry \n");
//             return -1;
//         } else {
//             int same;
//             same = strncmp(token, de.name, len_token);
//             //printf("====================================== the strncmp returned %d \n", same);
//             if (same == 0){
//                 printf("Found token: %s in file %s \n", token, de.name);
//                 /* find the curent locations, inode => get_inode(n) */
//                 int next_location;
//                 next_location = de.inode;
//                 printf("this is where i should go next: %d \n", next_location);
//                 fseek(file, BS*next_location, SEEK_SET);
//                 fread(&de, 1, sizeof(struct dir_entry), file);
//                 printf(" inode data located: %d \n", de.inode);
//                 printf(" dir_entry length: %d \n", de.rec_len);
//                 printf(" type : %d \n", de.file_type);
//                 printf(" actual name: %s \n", de.name);


//                 /* check if currently im looking at a dir or a file*/

//                 /* if im a dir then look for its data block */
//             }
//         }
//         prev_sde += de.rec_len;
//         return traverse(prev_sde, token);
//     }
// }

// int main(int argc, char *argv[]){

//     file = fopen("onedirectory.img", "r");
//     /*reconstructiong the super block*/
//     fseek(file, BS, SEEK_SET);
//     fread(&sb, 1, sizeof(struct super_block), file);
//     printf("------------------------------- SUPER BLOCK----------------------- \n");
//     printf(" Inodes count %d \n", sb.s_inodes_count);
//     printf(" Blocks count %d \n", sb.s_blocks_count);
//     printf(" Free blocks count %d \n", sb.s_free_blocks_count);
//     printf(" Free inodes count %d \n", sb.s_free_inodes_count);

//     /* reconstructing the group descriptor, which is at block #2 */
//     fseek(file, BS*2, SEEK_SET);
//     fread(&gd, 1, sizeof(struct group_desc), file);
//     printf("------------------------------- GROUP DESCRIPTOR------------------ \n");
//     printf("Blocks bitmap block  %d \n", gd.bg_block_bitmap);
//     printf("Inodes bitmap block  %d \n", gd.bg_inode_bitmap);
//     printf("Inodes table block   %d \n", gd.bg_inode_table);
//     printf("Free blocks count  %d \n", gd.bg_free_blocks_count);
//     printf("Free inodes count  %d \n", gd.bg_free_inodes_count);
//     printf("Directories count %d \n", gd.bg_used_dirs_count);

//     /* reconstructing the Inode bitmap, which is at block #4*/
//     printf("------------------------------- INODE BITMAP ------------------ \n");
//     int imap_location;
//     imap_location = gd.bg_inode_bitmap;
//     fseek(file, BS*imap_location, SEEK_SET);
//     fread(&inode_bitmap, 1, sizeof(char)*16, file);
//     printf("inode bitmap: %s \n", inode_bitmap);

//     /* reconstructing the data bitmap, which is at block #3*/
//     printf("------------------------------- DATA BITMAP ------------------ \n");
//     int dmap_location, node_count;
//     dmap_location = gd.bg_block_bitmap;
//     node_count = sb.s_inodes_count;
//     fseek(file, BS*dmap_location, SEEK_SET);
//     fread(&data_bitmap, 1, sizeof(char)*node_count, file);
//     printf(" data bitmap: %s \n", data_bitmap);

//     printf("------------------------------- INODE ------------------ \n");
//     /* Rescontruct the inode table, which is at block#5. This will lead me to root */
//     int itable;
//     itable = gd.bg_inode_table;
//     fseek(file, BS*itable + 128, SEEK_SET);
//     fread(&in, 1, sizeof(struct inode), file);
//     /*since we know from super block that there are 16 inodes*/
//     printf("Inode data block first pointer to a block %d \n", in.i_block[0]); /* Pointers to blocks */
//     printf("Inode data block ptr 2 %d \n", in.i_block[1]);
//     printf("Inode data block ptr 3 %d \n", in.i_block[2]);
//     printf("Inode data block ptr 4 %d \n", in.i_block[3]);
//     printf("Inode data block ptr 5  %d \n", in.i_block[4]);

//     printf("------------------------------- ROOT ------------------ \n");
//     /*Reconstructing root*/
//     int root;
//     root = in.i_block[0];
//     fseek(file, BS*root, SEEK_SET);
//     fread(&de, 1, sizeof(struct dir_entry), file);
//     printf("read the dir_entry for root \n");
//     printf("roots inode data located: %d \n", de.inode);
//     printf("roots dir_entry length: %d \n", de.rec_len);
//     printf("roots type : %d \n", de.file_type);
//     printf("roots actual name: %s \n", de.name);

//     printf("------------------------------- NEXT ENTRY AFTER ROOT ------------------ \n");
//     /*get the size of the previous directory entry*/
//     int prev_sde = de.rec_len;
//     fseek(file, (BS*7)+prev_sde, SEEK_SET);
//     fread(&de, 1, sizeof(struct dir_entry), file);
//     printf(" inode data located: %d \n", de.inode);
//     printf(" dir_entry length: %d \n", de.rec_len);
//     printf(" type : %d \n", de.file_type);
//     printf(" actual name: %s \n", de.name);

//     printf("------------------------------- NEXT ENTRY AFTER 2 ENTRY------------------ \n");
//     /*get the size of the previous directory entry*/
//     int prev_sde2 = prev_sde+de.rec_len;
//     printf("size of fist entry + second entry %d \n", prev_sde2);
//     fseek(file, (BS*7)+prev_sde2, SEEK_SET);
//     fread(&de, 1, sizeof(struct dir_entry), file);
//     printf(" inode data located: %d \n", de.inode);
//     printf(" dir_entry length: %d \n", de.rec_len);
//     printf(" type : %d \n", de.file_type);
//     printf(" actual name: %s \n", de.name);

//     printf("------------------------------- NEXT ENTRY AFTER 3 ENTRY------------------ \n");
//     /*get the size of the previous directory entry*/
//     int prev_sde3 = prev_sde2+de.rec_len;
//     fseek(file, (BS*7)+prev_sde3, SEEK_SET);
//     fread(&de, 1, sizeof(struct dir_entry), file);
//     printf(" inode data located: %d \n", de.inode);
//     printf(" dir_entry length: %d \n", de.rec_len);
//     printf(" type : %d \n", de.file_type);
//     printf(" actual name: %s \n", de.name);
//     printf("------------------------------- GOING INSIDE TESTDIR ------------------ \n");
//     fseek(file, (BS*de.inode), SEEK_SET);
//     fread(&de, 1, sizeof(struct dir_entry), file);
//     printf(" inode data located: %d \n", de.inode);
//     printf(" dir_entry length: %d \n", de.rec_len);
//     printf(" type : %d \n", de.file_type);
//     printf(" actual name: %s \n", de.name);

//     printf("------------------------------- BLOCK 21------------------ \n");
//     /*get the size of the previous directory entry*/
//     fseek(file, (BS*21), SEEK_SET);
//     fread(&de, 1, sizeof(struct dir_entry), file);
//     printf(" inode data located: %d \n", de.inode);
//     printf(" dir_entry length: %d \n", de.rec_len);
//     printf(" type : %d \n", de.file_type);
//     printf(" actual name: %s \n", de.name);


//     printf("---------------------CALL TO TRAVERSING \n");
//     //int rec;
//     //rec = traverse(0, "lost+found");
//     //printf("%d \n", rec);

//     printf("---------------------CALLING STRTOK \n");
//     int len;
//     len = strlen("lost+found/testfile.txt");
//     printf("testing len function %d \n", len);

//     char *path = "/lost+found";
//     strip_path(path);


//     return(0);
// }