
CXX=gcc
SDL2CFLAGS=-I/usr/local/include/SDL2 -D_REENTRANT

CXXFLAGS=-O2 -c -Wall $(SDL2CFLAGS)
LDFLAGS=-L/usr/local/lib -lSDL2

BUILDDIR=build

exec:
	mkdir -p $(BUILDDIR)
	$(CXX) -shared -w -o $(BUILDDIR)/joy_app.dll src/joy_app.c -Wl,--out-implib,libappdll.a
	$(CXX) -shared -w -Iincludes `sdl2-config --cflags --libs` -lGL -o $(BUILDDIR)/sdl_joy_opengl.dll src/sdl_joy_opengl.c -Wl,--out-implib,libgldll.a
	$(CXX) -w -o $(BUILDDIR)/test src/sdl_joy.c `sdl2-config --cflags --libs`
