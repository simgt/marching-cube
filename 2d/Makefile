UNAME = $(shell uname)
CC = clang++

CFLAGS = -W -Wall -O0 -std=c++0x

ifeq ($(UNAME), Darwin)
LDFLAGS = -lglfw -framework Cocoa -framework OpenGL
else
LDFLAGS = -lglfw
endif

BIN = 2d
SRC = main.cc mesh.cc program.cc

## compile ##

OBJ = $(addprefix src/, $(SRC:.cc=.o))

all : $(BIN)

$(BIN) : $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o : %.cc
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(OBJ:.o=.d)

## clean ##

clean:
	find . -name '.DS_Store' | xargs rm -vf
	find . -name '*.o' | xargs rm -vf
	find . -name '*.d' | xargs rm -vf
	find . -name '*~' | xargs rm -vf
	rm -vf $(BIN)

check: all
	./$(BIN)
