
SDLPATH=..\\includes\\SDL2-2.26.4\\i686-w64-mingw32
INCLUDES=-I..\includes\\ -I$(SDLPATH)\\include
LIBPATH=-L$(SDLPATH)\lib

BUILDDIR=build

exec:
	mkdir -p $(BUILDDIR)
	cp $(SDL_path)\\bin\\SDL2.dll $(BUILDDIR)
	gcc -shared -w -o $(BUILDDIR)\\joy_app.dll src\\joy_app.c -Wl,--out-implib,$(BUILDDIR)\\libappdll.a
	gcc -shared -w $(INCLUDES) $(LIBPATH) -lmingw32 -lSDL2main -lSDL2 -o $(BUILDDIR)\\sdl_joy_opengl.dll src\\sdl_joy_opengl.c -Wl,--out-implib,$(BUILDDIR)\\libgldll.a
	gcc-w src\\sdl_joy.c $(INCLUDES) $(LIBPATH) -Wall -lmingw32 -lSDL2main -lSDL2 -o $(BUILDDIR)\\sdl_joy

