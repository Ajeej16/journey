@echo off

set application_name=journey
set build_options=
set includes=-I..\includes\
set compiler_flags=-nologo -Zi -FC -wd4533 -wd4700 -GS- -Gs99999999
set linker_flags=/LIBPATH:..\libs -opt:ref -incremental:no
set platform_linker_flags=%linker_flags% user32.lib gdi32.lib winmm.lib

if not exist build mkdir build 
pushd build

call vcvarsall.bat x64

xcopy /y ..\libs\libfftw3f-3.dll .

cl %compiler_flags% %includes% ..\src\joy_app.c -LD -MT -D_USRDLL /link %linker_flags% -EXPORT:InitApp -EXPORT:UpdateAndRender /out:journey_app.dll

cl %compiler_flags% %includes% ..\src\win32_joy_opengl.c -LD -MT /link %platform_linker_flags% -EXPORT:InitRenderer -EXPORT:StartFrame -EXPORT:EndFrame opengl32.lib /out:win32_journey_opengl.dll

cl %compiler_flags% %includes% ..\src\win32_joy.c /link %platform_linker_flags% /out:%application_name%.exe

popd