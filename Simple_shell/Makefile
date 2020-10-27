PRGNAME = ss
SS_SRC = main.c SimpleShell.c 
all: $(PRGNAME)

$(PRGNAME): $(SS_SRC)
	$(CC) $(SS_SRC) -o $(PRGNAME)

run: $(PRGNAME)
	./$(PRGNAME)

clean:
	rm --recursive --force $(PRGNAME)

.PHONY: run clean