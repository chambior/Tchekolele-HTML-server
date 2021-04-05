
FILES = struct_arbre.o api.o main.o parse.o
DFLAGS =
DFILES = struct_arbre.o api.o main.o parse.o
DIST = Archive.tar.gz

main: $(FILES)
	gcc -g -Wall -o $@ $^ $(DFLAGS)
%.o: %.c
	gcc -g -Wall -c $< $(DFLAGS)

debug: clear clean dflags main

dflags:
	$(DFLAGS) = -DDEBUG

.PHONY: clear
clear:
	clear

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf main

dist:
	tar -cvzf $(DIST) *
