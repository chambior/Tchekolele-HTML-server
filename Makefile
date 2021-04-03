main: struct_arbre.o api.o main.o parse.o
	gcc -g -Wall -o $@ $^
%.o: %.c
	gcc -g -Wall -c $<
clear:
	clear
clean:
	rm -rf *.o
	rm -rf main
