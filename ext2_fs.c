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

/*Return where there is enough space to write a new dir_entry for the file to be copy */
int find_free_entry(char *img, int block_num){
    printf("FINDING A FREE dir_entry \n");

    if((file = fopen(img, "r")) == NULL){
        perror("Error opening the image in finding a free entry: \n");
        exit(1);
    }

    int des = 0; /* directory entry size */
    while(des < BLOCK_SIZE){
        fseek(file, BLOCK_SIZE*block_num+des, SEEK_SET);
        fread(&de, 1, sizeof(struct dir_entry), file);
        printf(" inode data located: %d \n", de.inode);
        printf(" dir_entry length: %d \n", de.rec_len);
        printf(" type : %d \n", de.file_type);
        printf(" actual name: %s \n", de.name);

        des += de.rec_len;
    }
    return de.inode;
}

int get_data_from_inode(char *img, int inode_num){
    inode_num --;

    if((file = fopen(img, "r")) == NULL){
        perror("Error opening the image: \n");
        exit(1);
    }

    fseek(file, BLOCK_SIZE*LOC_I_TABLE+INODE_SIZE*inode_num, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), file);

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
            break;
        }

        size += de.rec_len;

    }
    printf("this is the inode that im found %d \n", de.inode);
    return de.inode;

}

/* returns the location of the data for this final path */
int strip_path(char *img, char *path){
    int path_len, loc_inode, next_inode, next_data_block;
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
    int i;
    for(i = 0; i < 16; i++){ 
        if((i % 8) == 0) 
            printf(" "); 
        if (ibm.inodes & 1) 
            printf("1"); 
        else 
            printf("0");

        ibm.inodes >>= 1; 
    } 
    printf("\n");
}

int find_free_inode(struct inode_bitmap ibm){

    struct inode_bitmap ibm_cp;
    ibm_cp = ibm;
    //print_ibm(ibm);
    int i;
    for(i = 0; i < 16; i++){ 
        if (!(ibm_cp.inodes & 1))
            return i;
        ibm_cp.inodes >>= 1; 
    } 
    printf("\n");    
    return -1;
}

void print_dbm(struct data_bitmap dbm){  
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

        dbm.data1 >>= 1;
    } 
    printf("\n");

    /* the remaining 64 bits since the struct has that much space*/
    printf("This is the data2 bit map \n");
    int j;
    for(j = 0; j  < 64; j++){ 
        if((j % 8) == 0) 
            printf(" "); 
        if (dbm.data2 & 1) 
            printf("1"); 
        else 
            printf("0"); 

        dbm.data2 >>= 1;
    } 
    printf("\n");
}

int free_data_inode(struct data_bitmap dbm){  
    struct data_bitmap dbm_cp;
    dbm_cp = dbm;

    int i;
    /* first 64 bits since the struct has that much space*/
    for(i = 0; i < 64; i++) { 
        if (!(dbm_cp.data1 & 1))
            return i;
        dbm_cp.data1 >>= 1;
    } 
    printf("\n");

    /* the remaining 64 bits since the struct has that much space*/
    int j;
    for(j = 0; j  < 64; j++){ 
        if (!(dbm_cp.data2 & 1))
            return j;
        dbm_cp.data2 >>= 1;
    } 
    printf("\n");

    return -1;
}

char *strip_name(char *src_path){
    int path_len;
    path_len = strlen(src_path);
    char path_cp[path_len];
    strcpy(path_cp, src_path);
    char s[2] = "/";
    char *token;
    char *prev_token;
   
   /* get the first token */
   token = strtok(path_cp, s); 

   while(token != NULL) {
        strcpy(prev_token, token);
        token = strtok(NULL, s);
        if(token == 0){
            printf("RETURN FILE NAME: %s \n", prev_token);
            return prev_token;
        }
    }
    return src_path;
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