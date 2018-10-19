CFLAGS = -Wall -Wextra -pedantic -std=c99 -fPIC -fvisibility=hidden
LDFLAGS = -shared
TARGET_LIB = libmalloc.so
VPATH = src
OBJS = malloc.o calloc.o realloc.o free.o chunk_tools.o

.PHONY: all ${TARGET_LIB} clean

${TARGET_LIB}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^

all: ${TARGET_LIB}

check: ${TARGET_LIB}
	tests/test_suite.sh

clean:
	${RM} ${TARGET_LIB} ${OBJS}
