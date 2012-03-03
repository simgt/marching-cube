CC = g++-mp-4.6
#CC = clang++-mp-3.0
CFLAGS = -W -Wall -O0 -std=c++0x \
		-DTBB_IMPLEMENT_CPP0X=1 \
		-Isource -I/opt/local/include -I/usr/local/include \
		$(addprefix -I, /Library/Frameworks/Horde3D.framework/Headers /Library/Frameworks/Horde3DUtils.framework/Headers)
LDFLAGS = -lglfw -ltbb \
		$(addprefix -framework , Horde3D Horde3DUtils OpenGL Cocoa)

BIN = build/sculpt
SRC = main.cc util/h3d.cc \
	  map/map.cc map/marching_cube.cc

## compile ##

OBJ = $(addprefix source/, $(SRC:.cc=.o))

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
