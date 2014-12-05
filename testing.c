./ext2cp ./onedirectory.img ./gil2.txt /testdirectory/testf

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