# lab4 Makefile
# Sreenivas Jeevan Nadella
# Based on provided Makefile

# Global variables
CC=gcc
LD=ld
WARNS=-Wall -Wextra -pedantic
CFLAGS=-g3 -std=gnu99 ${WARNS}
LIBS=-lpthread

all: tags headers matrix

# This builds visual symbol (.vs) files and the header files.
headers: *.c tags
	./headers.sh

# Tags (for C code) are too handy not to keep up to date.
# Vim users: use ctags with vim via Control-].
# Emacs users: use etags with emacs (etags command).
# Comment out the command that you're NOT using.
tags: *.c
	ctags -R .
#   etags -R .


# This is a link rule, we have a universal compile rule down below
# Output is the target of the rule : -o $@
# I want to link all of the dependencies: $^
matrix: matrix.o
	${CC} -g -o $@ $^ ${LIBS}

matrix.o: matrix.c
	${CC} -g -c $<

# This is our master compiler rule to generate .o files.
# It needs all 4 warnings (see WARNS variable defined above)
%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -r *.vs *.o *.h headers_timestamp tags matrix

zip:
	zip -r lab4.zip matrix.c Makefile README.txt headers.sh headers.awk

tar:
	tar cfpv lab4.tar matrix.c Makefile README.txt headers.sh headers.awk

gz:
	gzip lab4.tar
