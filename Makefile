COMPILE=g++
FLAGS=-Wall -Werror -ansi -pedantic

all: rshell
	
rshell: rshell.o
  $(COMPILE) rshell.o -o rshell 

rshell.o: rshell.cpp 
	$(COMPILE) -c -Wall -Werror -ansi -pedantic rshell.cpp 
