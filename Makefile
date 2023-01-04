JAVAC=javac
CC=gcc
CFLAGS=-Werror -Wall

vm_test: Main.class class_file.o vm.o instructions.o
	$(CC) $(CFLAGS)  -lm ./build/instructions.o ./build/vm.o ./build/class_file.o -o ./build/vm_test vm_test.c
	cd ./build && ./vm_test

Main.class: Main.java
	$(JAVAC) -d build Main.java

class_file.o: class_file.c class_file.h types.h byteorder.h
	$(CC) $(CFLAGS) -c -o ./build/class_file.o class_file.c

vm.o: vm.c vm.h types.h class_file.h
	$(CC) $(CFLAGS) -c -o ./build/vm.o vm.c

instructions.o: instructions.c instructions.h class_file.h vm.h
	$(CC) $(CFLAGS) -c -o ./build/instructions.o instructions.c

class_file_test: class_file.o
	$(CC) $(CFLAGS) -lm ./build/class_file.o -o ./build/class_file_test class_file_test.c
	cd ./build && ./class_file_test

clean:
	rm build/*