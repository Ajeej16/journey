
CXX=g++
SDL2CFLAGS=-I/usr/local/include/SDL2 -D_REENTRANT

CXXFLAGS=-O2 -c -Wall $(SDL2CFLAGS)
LDFLAGS=-L/usr/local/lib -lSDL2

BUILDDIR=build

exec:
	mkdir -p $(BUILDDIR)
	$(CXX) -o $(BUILDDIR)/test src/sdl_joy.c `sdl2-config --cflags --libs`
