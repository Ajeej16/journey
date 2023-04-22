BUILDDIR=build
LIBPATH=libs/win32/
RLDFLAGS=-lglfw3dll -lopengl32 -lmingw32
PLDFLAGS=-lglfw3dll -lgdi32 -luser32 -lkernel32
DLL=dll
PC=

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
LIBPATH=libs/mac/
RLDFLAGS = $(pkg-config --libs glfw3) -framework Cocoa -framework OpenGL -framework IOKit
PLDFLAGS = $(pkg-config --libs glfw3) -framework Cocoa -framework IOKit
DLL=dylib
PC=$(pkg-config --cflags glfw3)
endif

INCLUDES=-Iincludes/
LIB=-L$(BUILDDIR)

all:
	mkdir -p $(BUILDDIR)
	cp $(LIBPATH)* $(BUILDDIR)
	gcc -g -shared -w -o $(BUILDDIR)/joy_app.$(DLL) src/joy_app.c
	gcc -g -shared -w $(INCLUDES) $(LIB) -o $(BUILDDIR)/glfw_joy_opengl.$(DLL) src/glfw_joy_opengl.c $(RLDFLAGS)
	gcc $(PC) -g -w -Wall src/glfw_joy.c -o $(BUILDDIR)/glfw_joy $(INCLUDES) $(LIB) $(PLDFLAGS) $(BUILDDIR)/joy_app.$(DLL) $(BUILDDIR)/glfw_joy_opengl.$(DLL)

