PRJ=project1

NAME=hinfosvc
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -lm -fcommon

run: hinfosvc.c
	${CC} ${CFLAGS} hinfosvc.c -o ${NAME} 

.PHONY: clean
clean:
	rm -f hinfosvc

.PHONY: pack
pack: clean
	zip xphanj00.zip hinfosvc.c Makefile