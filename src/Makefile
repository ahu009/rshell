COMPILE=g++
FLAGS=-Wall -Werror -ansi -pedantic

all: rshell move

rshell: rshell.o
	$(COMPILE) rshell.o -o rshell 

rshell.o: rshell.cpp 
	$(COMPILE) -c -Wall -Werror -ansi -pedantic rshell.cpp 

move:	
	mv rshell ../bin
