main: main.o api.o struct_arbre.o parse.o
	gcc -Wall -o $@ $^
%.o: %.c
	gcc -Wall -c $<
clean:
	rm -rf *.o
	rm -rf main
