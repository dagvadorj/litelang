all: compile install

compile: slist.o token.o exec.o liten.o main.o
	gcc slist.o token.o exec.o liten.o main.o -o litelang

slist.o: slist.c
	gcc -c slist.c
	
token.o: token.c
	gcc -c token.c

exec.o: exec.c
	gcc -c exec.c

liten.o: liten.c
	gcc -c liten.c

main.o: main.c
	gcc -c main.c

install: 
	cp litelang /usr/bin

clean:
	rm -rf *o
	rm litelang
remove:
	rm /usr/bin/litelang