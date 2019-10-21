

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)

default: prova 

# To create the executable file myprog we need the object files
# main.o, calculator.o, counter.o:

prova: prova.o 
	gcc prova.o -o prova 

# Creating object files
prova.o: prova.c  
	gcc -c prova.c

