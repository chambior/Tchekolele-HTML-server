main: struct_arbre.o api.o main.o parse.o
	gcc -Wall -o $@ $^
%.o: %.c
	gcc -Wall -c $<
clear:
	clear
clean:
	rm -rf *.o
	rm -rf main
