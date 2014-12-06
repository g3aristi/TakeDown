all : ext2_cp ext2_ln

ext2_cp : ext2_cp.o ext2_fs.o
	gcc -Wall -g -o ext2cp $^

ext2_lin : ext2_ln.o ext2_fs.o
	gcc -Wall -g -o ext2ln $^

%.o : %.c ext2_fs.h
	gcc -Wall -g -c $^

clean :
	rm *.o ext2_cp
