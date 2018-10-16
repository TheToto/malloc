CFLAGS = -Wall -Wextra -pedantic -std=c99 -fPIC -fvisibility=hidden
LDFLAGS = -shared
TARGET_LIB = libmalloc.so
VPATH = src
OBJS = malloc.o

.PHONY: all ${TARGET_LIB} clean

${TARGET_LIB}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^

all: ${TARGET_LIB}

clean:
	${RM} ${TARGET_LIB} ${OBJS}
