all :  ext2_fs

ext2_fs : ext2_fs.o
	gcc -Wall -g -o ext2 ext2_fs.c ext2_fs.h
