# "Hello World" Makefile
# Sreenivas Jeevan Nadella
# Based on provided Makefile

# Global variables
CC=gcc
LD=ld
WARNS=-Wall -Wextra -pedantic
CFLAGS=-g3 ${WARNS}

# hello is an executable I want to build, the rest are handy things
all: tags headers hello

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
hello: hello.o
	${CC} -g -o $@ $^

hello.o: hello.c
	${CC} -g -c $<

# This is our master compiler rule to generate .o files.
# It needs all 4 warnings (see WARNS variable defined above)
%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@
