# This is the makefile that generates the executable

# Files to compile
FILES_C = red-black-tree.c create-tree.c read-write-tree.c consult-tree.c tree-to-mmap.c dbfnames-mmap.c main.c 
# FILES_C = create-tree.c

# Object files
FILES_O = red-black-tree.o create-tree.o read-write-tree.o consult-tree.o tree-to-mmap.o dbfnames-mmap.o main.o

# Exectuable to generate
TARGET = main

# Compilation options
CFLAGS = -Wall -Werror -g

# Linker options 
LFLAGS = -lm

# There is no need to change the instructions below this
# line. Change if you really know what you are doing.


$(TARGET): $(FILES_O) Makefile
	gcc $(FILES_O) -o $(TARGET) $(LFLAGS) 

%.o: %.c Makefile
	gcc $(CFLAGS) -c $<

all: $(TARGET) 

prepare:
	/bin/rm read-write-tree.c consult-tree.c main.c read-write-tree.h consult-tree.h tree-to-mmap.h dbfnames-mmap.h

clean:
	/bin/rm create-tree.o $(TARGET) 
