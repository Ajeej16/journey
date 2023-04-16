BUILDDIR=build
LIBPATH=libs/win32/
RLDFLAGS=-lglfw3dll -lopengl32 -lmingw32
PLDFLAGS=-lglfw3dll -lgdi32 -luser32 -lkernel32

ifeq "$(OSTYPE)" "darwin"
LIBPATH=libs/mac/
RLDFLAGS = -framework CoreFoundation -framework OpenGL $(BUILDDIR)/libglfw.3.dylib
PLDFLAGS = -framework CoreFoundation $(BUILDDIR)/libglfw.3.dylib
endif

INCLUDES=-Iincludes/
LIB=-L$(LIBPATH)

all:
	mkdir -p $(BUILDDIR)
	cp $(LIBPATH)* $(BUILDDIR)
	gcc -g -shared -w -o $(BUILDDIR)/joy_app.dll src/joy_app.c -Wl,--out-implib,$(BUILDDIR)/libappdll.a
	gcc -g -shared -w $(INCLUDES) $(LIB) -o $(BUILDDIR)/glfw_joy_opengl.dll src/glfw_joy_opengl.c -Wl,--out-implib,$(BUILDDIR)/libgldll.a $(RLDFLAGS)
	gcc -g -w -Wall src/glfw_joy.c -o $(BUILDDIR)/glfw_joy $(INCLUDES) $(LIB) $(PLDFLAGS) $(BUILDDIR)/joy_app.dll

