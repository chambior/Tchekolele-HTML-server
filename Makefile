main: struct_arbre.o api.o main.o 
	gcc -Wall -o $@ $^
%.o: %.c
	gcc -Wall -c $<
clean:
	rm -rf *.o
	rm -rf main
