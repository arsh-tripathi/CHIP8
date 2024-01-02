EXEC = emulator

CC = g++

CFLAGS = -g -MMD -I src/include 

LIBS = -L src/lib -lmingw32 -lSDL2main -lSDL2

SRC = $(wildcard src/*.cpp)

OBJECTS = $(SRC:.cpp=.o)

DEPENDS = $(OBJECTS:.o=.d)

${EXEC}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} ${LIBS} -o ${EXEC}

-include ${DEPENDS}

# all:
# 	gcc -I src/include -L src/lib -o main main.c -lmingw32 -lSDL2main -lSDL2

.PHONY: clean

clean:
	rm *.o *.d ${EXEC}