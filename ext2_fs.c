#include <stdio.h>
#include <stdlib.h>
#include "ext2_fs.h"

FILE *file;
char sb[20];

int main(int argc, char *argv[]){

    file = fopen("emptydisk.img", "r");
    int inode_count;
    fseek(file, 1024, SEEK_SET);
    fread(&inode_count, 1, sizeof(int), file);
    printf("%d \n", inode_count);

    return(0);
}