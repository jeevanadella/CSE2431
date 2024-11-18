# lab2 Makefile
# Sreenivas Jeevan Nadella
# Based on provided Makefile

# Global variables
CC=gcc
LD=ld
WARNS=-Wall -Wextra -pedantic
CFLAGS=-g3 -std=gnu99 ${WARNS}
LIBS=-lpthread

TARGET = bounded_buffer
SRCS = main.c buffer.c
OBJS = $(SRCS:.c=.o)

# bounded_buffer is an executable I want to build, the rest are handy things
all: tags headers bounded_buffer

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

# This is our master compiler rule to generate .o files.
# It needs all 4 warnings (see WARNS variable defined above)
%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	rm -r *.vs *.o
