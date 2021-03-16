HASHDIR = ./uthash/include

CFALGS += -I$(HASHDIR)

main: main.c $(HASHDIR)/uthash.h
	gcc main.c $(CFALGS) -l curl -o main