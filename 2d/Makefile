CC = clang++
CFLAGS = -W -Wall -O0 -std=c++0x \
		-Isrc \
		$(shell sdl-config --cflags)
LDFLAGS = $(shell sdl-config --libs)

BIN = 2d
SRC = main.cc

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
