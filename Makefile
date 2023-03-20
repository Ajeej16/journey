
CXX=gcc
SDL2CFLAGS=-I/usr/local/include/SDL2 -D_THREAD_SAFE

CXXFLAGS=-O2 -c -Wall $(SDL2CFLAGS)
LDFLAGS=-L/usr/locla/lib -lSDL2

BUILDDIR=build

exec:
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) src/sdl_joy.c -o ../$(BUILDDIR)/sdl_joy

clean:
	rm -rf build