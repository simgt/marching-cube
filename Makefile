CC = clang++
CSTD = c++0x
CFLAGS = -W -Wall -O0 \
		-Isrc \
		$(addprefix -I, /Library/Frameworks/Horde3D.framework/Headers /Library/Frameworks/Horde3DUtils.framework/Headers)
LDFLAGS = -lglfw \
		$(addprefix -framework , Horde3D Horde3DUtils OpenGL Cocoa)

BIN = build/fieldcraft
SRC = main.cc generator.cc

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
