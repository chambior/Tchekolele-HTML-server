main: test.o struct_arbre.o
	gcc -Wall -o $@ $^
%.o: %.c
	gcc -Wall -c $<
clean:
	rm -rf *.o
	rm -rf main
