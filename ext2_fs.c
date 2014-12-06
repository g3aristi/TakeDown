#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext2_fs.h"

#define BS 1024 /*BLOCK SIZE*/
#define FDEAR 24 /*First Directory Entry After Root*/

FILE *file, *file2;
struct dir_entry de;
struct inode in;


char inode_bitmap[16];
char data_bitmap[16];
char *path;

/* updates the inodes block pointers to the new blokcs where its data is now */
void update_inode(char *img, int in_index, int db){

    int i;
    if((file = fopen(img, "r+")) == NULL){
        perror("Error opening the image in finding a free entry: \n");
        exit(1);
    }

    fseek(file, BLOCK_SIZE*LOC_D_BITMAP+INODE_SIZE*in_index, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), file);
    for( i = 0; i < 15; i++){
        if (         (            (in.i_block[i]  == 0)     )            ){
            in.i_block[i] = db;
        }
    }
}

void print_dbm(struct data_bitmap dbm){  
    int i;
    printf("This is the data1 bit map \n");
    /* first 64 bits since the struct has that much space*/
    for(i = 0; i < 64; i++){ 
        //printf("this is where the struct data bit is at: %d \n", );
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


/* THis functions do not work properly, they change the bitmaps in the wrong way */
// void update_inode_bitmap(struct inode_bitmap ibm, int index){
//     int i;
//     /* first 64 bits since the struct has that much space*/
//     if(index < 16){
//         for(i = 0; i < 16; i++) { 
//             if ( i == index)
//                 ibm.inodes |= 1<< (index-1);
//         } 
//     }
// }


// /* updates the data bitmap to show that a new data block is in use */
// void update_data_bitmap(struct data_bitmap dbm, int index){
//     int i, j;
//     /* first 64 bits since the struct has that much space*/
//     if(index < 64){
//         for(i = 0; i < 64; i++) { 
//             if ( i == index)
//                 dbm.data1 |= 1<< (index-1);
//         } 
//     }

//     /* the remaining 64 bits since the struct has that much space*/
//     if(index >= 64){
//         for(j = 0; j  < 64; j++){ 
//             if ( i == index )
//             dbm.data1 |= 1<< (index-1);
//         } 
//     } 
// }


int free_data_inode(struct data_bitmap dbm){  
    struct data_bitmap dbm_cp;
    dbm_cp = dbm;

    int i;
    /* first 64 bits since the struct has that much space*/
    for(i = 0; i < 64; i++) { 
        if (!(dbm.data1 & 1))
            return i;
        dbm_cp.data1 >>= 1;
    } 
    printf("\n");

    /* the remaining 64 bits since the struct has that much space*/
    int j;
    for(j = 0; j  < 64; j++){ 
        if (!(dbm.data2 & 1))
            return j;
        dbm_cp.data2 >>= 1;
    } 
    printf("\n");

    return -1;
}

/* writes data from source to destination given the file size and the free data block.
    returns 1 when the writting was successfull and -1 other wise */
int write_data(char *img, char *src, int file_size, int fdb, struct inode in, 
    struct data_bitmap dbm, struct inode_bitmap ibm, int fin){ /* free inode */

    if((file = fopen(img, "r+")) == NULL){
        perror("Error opening the image in finding a free entry: \n");
        exit(1);
    }

    if((file2 = fopen(src, "r")) == NULL){
        perror("Error opening the source file: \n");
        exit(1);
    }

    if(file_size < BLOCK_SIZE){
        /* populate the buffer */
        char buff[file_size];
        fseek(file2, 0, SEEK_SET);
        fread(&buff, 1, file_size, file2);
        fseek(file, BLOCK_SIZE*fdb, SEEK_SET);
        fwrite(buff, 1, file_size, file);

        char new_buff[file_size];
        fseek(file, BLOCK_SIZE*fdb, SEEK_SET);
        fread(&new_buff, 1, file_size, file);
        return 1;
    }
    /* check if the file2 file needs multiple blocks */
    int i, num_bloks_needed, remeninder;
    int offset = 0;
    int new_db = fdb;
    remeninder = file_size;
    num_bloks_needed = file_size / BLOCK_SIZE;
    for(i = 0; i < num_bloks_needed; i++){

        /* if we are passed the first block find a new block to write to */
        /* update the inode of this the destination file */

        if( i > 0 && i < num_bloks_needed){
            new_db= free_data_inode(dbm) + 1;
            //update_data_bitmap(dbm, new_db);
            //print_dbm(dbm);

            /* update data bitmap*/
            //update_inode_bitmap(ibm, fin);

            //update_inode(in);
            //update_inode(img, fin, new_db);
        }

        /* populate the buffer */
        char buff[BLOCK_SIZE];
        /* find the begining of the file */
        fseek(file2, 0+offset, SEEK_SET);
        fread(&buff, 1, BLOCK_SIZE, file2);

        /* seek to the data block that i want to write to*/
        fseek(file, BLOCK_SIZE*new_db, SEEK_SET);
        fwrite(buff, 1, file_size, file);

        /* checking that i actually wrote to the img */
        char new_buff[file_size];
        fseek(file, BLOCK_SIZE*fdb, SEEK_SET);
        fread(&new_buff, 1, file_size, file);

        /* case where we have read most of the file2 data and the next buff its not a full
            1024 buff */
        if(remeninder < BLOCK_SIZE){
            char buff[remeninder];

            /* offset is how much of the file i have read */
            fseek(file2, 0+offset, SEEK_SET);
            fread(&buff, 1, remeninder, file2);

            /* seek to the data block that i want to write to*/
            fseek(file, BLOCK_SIZE*new_db, SEEK_SET);
            fwrite(buff, 1, remeninder, file);

            /* checking that i actually wrote to the img */
            char new_buff[remeninder];
            fseek(file, BLOCK_SIZE*fdb, SEEK_SET);
            fread(&new_buff, 1, remeninder, file);

        }
        remeninder -= BLOCK_SIZE;
        offset += BLOCK_SIZE;
    }

    return -1;
}


/*Return -1 if the new dir_entry was written to the cur dir_entry */
int add_free_entry(char *img, int block_num, struct dir_entry *new_de){

    if(block_num == 0){
        printf("incorrect block ++++++++++++++++++++++++++++++ \n");
        return -1;
    }

    if((file = fopen(img, "r+")) == NULL){
        perror("Error opening the image in finding a free entry: \n");
        exit(1);
    }
    int des = 0; /* directory entry size */
    while(des < BLOCK_SIZE){
        /* iterating through all the dir entries in this block */
        fseek(file, BLOCK_SIZE*block_num+des, SEEK_SET);
        fread(&de, 1, sizeof(struct dir_entry), file);

        printf("block number #: %d \n", block_num);

        /* if i find available space */
        int old_value, new_value;
        old_value = de.rec_len;
        if((de.name_len+8 < de.rec_len) && (des + de.rec_len == 1024)){

            /* Splitting the dir entry */
            de.rec_len = de.name_len+8; /* update the rec_len of the dir entry */
            new_value = de.rec_len;

            /* done spliting now check if there is still enough space, and 
                write the actual dir entry */
            if((old_value - new_value) > new_de->rec_len){
                /* seek to the previous dir entry and now add seek to that location 
                    plus its new rec_len */

                /* adding the padding */
                int before_padding;
                before_padding = new_de->rec_len;
                new_de->rec_len += old_value-new_value-before_padding;

                fseek(file, BLOCK_SIZE*block_num+des+new_value, SEEK_SET);
                /* SHOULD ADD PADDING */
                fwrite(new_de, 1, sizeof(struct dir_entry), file);
                /* reading that i wrote the actual new dir entry */
                // fseek(file, BLOCK_SIZE*block_num+des+new_value, SEEK_SET);
                // fread(&de, 1, sizeof(struct dir_entry), file);
                return 1;
            }
        }

    des += de.rec_len;
    }
    return -1;
}

int get_data_from_inode(char *img, int inode_num){
    inode_num --;

    if((file = fopen(img, "r")) == NULL){
        perror("Error opening the image: \n");
        exit(1);
    }

    fseek(file, BLOCK_SIZE*LOC_I_TABLE+INODE_SIZE*inode_num, SEEK_SET);
    fread(&in, 1, sizeof(struct inode), file);
printf("this is the data_block passed in #: %d \n", in.i_block[0]);
    /* should loop through all the data block pointers */
    return in.i_block[0];

}


/* return the inode of the data bloock of the the mathching file 
    where block_number is a dir*/
int find_inode(char *img, char *token, int block_num){
    //block_num --;
    printf("block %d, has token %s \n", block_num, token);
    char tk_cp[strlen(token+1)];
    strcpy(tk_cp, token);

    if((file = fopen(img, "r")) == NULL){
        perror("Error opening the image: \n");
        exit(1);
    }

    int size;
    size = 0;

    printf("THIS IS THE BLOCK NUMBER #: %d \n", block_num);
        printf("this is what im looking for %s \n", token);
        fseek(file, BLOCK_SIZE*block_num+size, SEEK_SET);
        fread(&de, 1, sizeof(struct dir_entry), file);
        printf(" inode data located: %d \n", de.inode);
        printf(" dir_entry length: %d \n", de.rec_len);
        printf(" type : %d \n", de.file_type);
        printf(" actual name: %s \n", de.name);
    /*create funtion that read all dir entries*/


    // while(size < BLOCK_SIZE){
    //     printf("THIS IS THE BLOCK NUMBER #: %d \n", block_num);
    //     printf("this is what im looking for %s \n", token);
    //     fseek(file, BLOCK_SIZE*block_num+size, SEEK_SET);
    //     fread(&de, 1, sizeof(struct dir_entry), file);
    //     printf(" inode data located: %d \n", de.inode);
    //     printf(" dir_entry length: %d \n", de.rec_len);
    //     printf(" type : %d \n", de.file_type);
    //     printf(" actual name: %s \n", de.name);
    //     if(size > 1){
    //         break;
    //     }

    //     if((strncmp(tk_cp, de.name, strlen(token))) == 0){
    //         break;
    //     }

    //     size += de.rec_len;

    // }
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