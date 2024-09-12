#
# SDL Makefile
#


BIN = cells.exe
CC = gcc
CFLAGS = -Wall -g
LIBS = mingw32 SDL2main SDL2_ttf SDL2

REL_CFLAGS = -Wall -O2

run: build
	@echo -e '### Running... ###\n'
	@./${BIN}

build:
	@echo '### Building... ###\n'
	${CC} ${CFLAGS} -o ${BIN} main.c $(addprefix -l,${LIBS})

