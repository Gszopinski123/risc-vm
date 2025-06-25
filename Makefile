all: clean build_test build 

CC = clang
CFLAGS = -Wall -g -I./include
TESTDIR = tests
OBJDIR = build/obj
EXEDIR = build/bin
build_test: 
	@for i in $(shell seq 1 9); do \
		riscv64-unknown-elf-as -march=rv32imac -o $(OBJDIR)/test$$i.o $(TESTDIR)/test$$i.s; \
		riscv64-unknown-elf-ld -m elf32lriscv -o $(EXEDIR)/test$$i.elf $(OBJDIR)/test$$i.o; \
	done

build: src/riscv_32.c
	$(CC) $(CFLAGS) src/riscv_32.c -o build/bin/riscv_32-vm
	echo "Compiled VM"

clean: 
	rm -rf $(EXEDIR)/*.elf
	rm -rf $(OBJDIR)/*.o

	