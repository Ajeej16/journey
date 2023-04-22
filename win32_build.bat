@echo off

set application_name=journey
set build_options=
set includes=-I..\includes\
set compiler_flags=-nologo -Zi -FC -wd4533 -wd4700 -GS- -Gs9999999
set linker_flags=-opt:ref -incremental:no user32.lib gdi32.lib
set platform_linker_flags=%linker_flags% winmm.lib

if not exist build mkdir build 
pushd build

call vcvarsall.bat

cl %compiler_flags% %includes% ..\src\joy_app.c -LD /link -EXPORT:InitApp -EXPORT:UpdateAndRender /out:journey_app.dll

cl %compiler_flags% %includes% ..\src\win32_joy_opengl.c -LD /link -EXPORT:InitRenderer -EXPORT:StartFrame -EXPORT:EndFrame %linker_flags% opengl32.lib /out:win32_journey_opengl.dll

cl %compiler_flags% %includes% ..\src\win32_joy.c /link %platform_linker_flags% /out:%application_name%.exe

popd