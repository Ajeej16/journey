@echo off

set SDL_path=..\includes\SDL2-2.26.4\i686-w64-mingw32
set includes=-I..\includes\ -I%SDL_path%\include
set lib_path=-L%SDL_path%\lib

if not exist build mkdir build
pushd build

xcopy %SDL_path%\bin\SDL2.dll .

gcc -shared -w -o joy_app.dll ..\src\joy_app.c -Wl,--out-implib,libappdll.a

gcc -shared -w %includes% %lib_path% -lmingw32 -lSDL2main -lSDL2 -o sdl_joy_opengl.dll ..\src\sdl_joy_opengl.c -Wl,--out-implib,libgldll.a

gcc -std=c17 -w ..\src\sdl_joy.c %includes% %lib_path% -Wall -lmingw32 -lSDL2main -lSDL2 -o sdl_joy

popd

pause
