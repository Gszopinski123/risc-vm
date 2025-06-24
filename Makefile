all: clean build_test build 

CC = clang
CFLAGS = -Wall -g -I./include
TESTDIR = tests
OBJDIR = build/obj
EXEDIR = build/bin
build_test: 
	riscv64-unknown-elf-as -march=rv32imac -o $(OBJDIR)/test1.o $(TESTDIR)/test1.s
	riscv64-unknown-elf-ld -m elf32lriscv -o $(EXEDIR)/test1.elf $(OBJDIR)/test1.o
	riscv64-unknown-elf-as -march=rv32imac -o $(OBJDIR)/test2.o $(TESTDIR)/test2.s
	riscv64-unknown-elf-ld -m elf32lriscv -o $(EXEDIR)/test2.elf $(OBJDIR)/test2.o

build: src/riscv_32.c
	$(CC) $(CFLAGS) src/riscv_32.c -o build/bin/riscv_32-vm
	echo "Compiled VM"

clean: 
	rm -rf *.elf
	rm -rf *.o

	