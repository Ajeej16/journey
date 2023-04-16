#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#ifdef _WIN32
#include "windows.h"

#define __LoadLibrary(fn) LoadLibraryA(fn)
#define __UnloadLibrary(hnd) FreeLibrary(hnd)
#define __LoadFunc(hnd, fun) GetProcAddress(hnd, fun)

#define __GetCurrentDirectory(len, buf) GetCurrentDirectory(len, buf)

#define RENDER_DLL "glfw_joy_opengl.dll"
#define RENDER_TEMP_DLL "glfw_joy_opengl_temp.dll"
#define APP_DLL "joy_app.dll"
#define APP_TEMP_DLL "joy_app_temp.dll"

#elif __APPLE__
#include "dlfcn.h"
#include "unistd.h"
#include "sys/syslimits.h"

#define __LoadLibrary(fn) dlopen(fn, RTLD_LAZY)
#define __UnloadLibrary(hnd) dlclose(hnd)
#define __LoadFunc(hnd, fun) dlsym(hnd, fun)

#define __GetCurrentDirectory(len, buf) getcwd(buf, len)

#define RENDER_DLL "glfw_joy_opengl.dylib"
#define RENDER_TEMP_DLL "glfw_joy_opengl_temp.dylib"
#define APP_DLL "joy_app.dylib"
#define APP_TEMP_DLL "joy_app_temp.dylib"

#endif

#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "glfw_joy_renderer.h"

#include "joy_load.h"

global u32 running = 1;

internal
UNLOAD_LIB()
{
    __UnloadLibrary(handle);
}

internal
LOAD_LIB()
{
    return __LoadLibrary(filename);
}

internal
LOAD_FUNCTION()
{
    return __LoadFunc(handle, funcName);
}

internal u32
CreateCopyFile(char *filename, char *newFilename)
{
    FILE *file, *copy;
    if((file = fopen(filename, "rb")) == NULL)
        return 0;
    if((copy = fopen(newFilename, "wb")) == NULL)
        return 0;

    fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *data = malloc(size);
    if(fread(data, size, 1, file) == 0) {
        fclose(file);
        fclose(copy);
        free(data);
        return 0;
    }
    fclose(file);

    if(fwrite(data, size, 1, copy) == 0) {
        fclose(copy);
        free(data);
        return 0;
    }
    fclose(copy);

    free(data);
    return 1;
}

internal
PLATFORM_READ_FILE(GLFWReadFile)
{
    FILE *file = NULL;
    if((file = fopen(path, "rb")) == 0) { /*error*/ }

    fseek(file, 0, SEEK_END);
    u64 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *data = malloc(size);
    u64 read = 0;
    if((read = fread(data, size, 1, file)) == 0) { /*error*/ }
    fclose(file);

    ((u8 *)data)[size-1] = 0;

    if(outSize) *outSize = size;
    *outData = data;
}

#include "joy_load.c"

int main(int argc, char *argv[])
{
    GLFWwindow *window;

    if(!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600,
                              "Journey GLFW",
                              NULL, NULL);
    if(!window) { glfwTerminate(); return 1; }

    char buildDir[PATH_MAX];
    __GetCurrentDirectory(PATH_MAX, buildDir);

    loaded_code renderCode = {0};
    render_function_table renderFunctions = {0};
    InitLoadedCode(&renderCode, (void **)&renderFunctions,
            renderFunctionNames,
            ARRAY_COUNT(renderFunctionNames),
            buildDir,
            RENDER_DLL,
            RENDER_TEMP_DLL);

    loaded_code appCode = {0};
    app_function_table appFunctions = {0};
    InitLoadedCode(&appCode, (void **)&appFunctions,
            appFunctionNames,
            ARRAY_COUNT(appFunctionNames),
            buildDir,
            APP_DLL,
            APP_TEMP_DLL);

    LoadCode(&renderCode, buildDir);
    LoadCode(&appCode, buildDir);

    ASSERT(renderCode.isValid && appCode.isValid);

    platform_functions platFunctions = {0};
    platFunctions.readFile = GLFWReadFile;

    render_buffer *rb = renderFunctions.initRenderer(window );

    input_state *inputState = InitInputState();

    appFunctions.initApp(&platFunctions, rb);

    while(!glfwWindowShouldClose(window))
    {
        renderFunctions.startFrame(window, rb);

        appFunctions.updateAndRender(rb, inputState);

        renderFunctions.endFrame(window, rb);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}