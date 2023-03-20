
#include <SDL2/SDL.h>

#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "sdl_joy_renderer.h"

#include "joy_load.h"

global u32 running = 1;

internal
UNLOAD_LIB()
{
    SDL_UnloadObject(handle);
}

internal
LOAD_LIB()
{
    return SDL_LoadObject(filename);
}

internal
LOAD_FUNCTION()
{
    return SDL_LoadFunction(handle, funcName);
}

internal u32
CreateCopyFile(char *filename, char *newFilename)
{
    SDL_RWops *in = SDL_RWFromFile(filename, "rb");
    if(!in)
        return 0;
    
    SDL_RWseek(in, 0, RW_SEEK_END);
    u64 size = SDL_RWtell(in);
    SDL_RWseek(in, 0, RW_SEEK_SET);
    
    void *data = malloc(size);
    
    if(SDL_RWread(in, data, size, 1) <= 0)
    {
        SDL_RWclose(in);
        return 0;
    }
    SDL_RWclose(in);
    
    SDL_RWops *ou = SDL_RWFromFile(newFilename, "wb");
    if(!ou)
        return 0;
    
    if(SDL_RWwrite(ou, data, size, 1) <= 0)
    {
        SDL_RWclose(ou);
        return 0;
    }
    SDL_RWclose(ou);
    
    free(data);
    
    return 1;
}

internal
PLATFORM_READ_FILE(SDLReadFile)
{
    void *data = 0;
    u64 size = 0;
    
    data = SDL_LoadFile(path, &size);
    
    if(outSize)
        *outSize = size;
    
    *outData = data;
}

#include "joy_load.c"

internal void
SDLProcessEvents(input_state *inputState)
{
    SDL_Event event = { 0 };
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT: 
            {
                running = 0;
            } break;
            
            case SDL_KEYUP:
            case SDL_KEYDOWN:
            {
                u64 keyCode = event.key.keysym.sym;
                u8 isDown = ((event.key.state & (1<<31)) == 0);
                u64 key = 0;
                u8 keyState = 0;
                
                // TODO(ajeej): Implement other keys
                if(keyCode >= 'A' && keyCode <= 'Z')
                    key = KEY_ID_A + (keyCode - 'A');
                else if(keyCode >= '0' && keyCode <= '9')
                    key = KEY_ID_0 + (keyCode - '0');
                else if(keyCode == SDLK_SPACE)
                    key = KEY_ID_SPACE;
                else if(keyCode == SDLK_LCTRL)
                    key = KEY_ID_CTRL;
                
                if(isDown)
                {
                    if((inputState->keyStates[key] & INPUT_DOWN) == 0)
                    {
                        keyState |= INPUT_PRESSED;
                    }
                    keyState |= INPUT_DOWN;
                }
                
                inputState->keyStates[key] = keyState;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        // TODO(ajeej): error
        return -1;
    }
    
    SDL_Window *window = SDL_CreateWindow("SDL Journey",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL);
    
    if(!window)
    {
        SDL_Quit();
        // TODO(ajeej): error
        return -1;
    }
    
    char *buildDir = SDL_GetBasePath();
    
    loaded_code renderCode = {0};
    render_function_table renderFunctions = {0};
    InitLoadedCode(&renderCode, (void **)&renderFunctions,
                   renderFunctionNames,
                   ARRAY_COUNT(renderFunctionNames),
                   buildDir,
                   "sdl_journey_opengl.dll",
                   "sdl_journey_opengl_temp.dll");
    
    loaded_code appCode = {0};
    app_function_table appFunctions = {0};
    InitLoadedCode(&appCode, (void **)&appFunctions,
                   appFunctionNames,
                   ARRAY_COUNT(appFunctionNames),
                   buildDir,
                   "journey_app.dll",
                   "journey_app_temp.dll");
    
    LoadCode(&renderCode, buildDir);
    LoadCode(&appCode, buildDir);
    
    ASSERT(renderCode.isValid && appCode.isValid);
    
    platform_functions platFunctions = {0};
    platFunctions.readFile = SDLReadFile;
    
    render_buffer *rb = renderFunctions.initRenderer(window);
    
    input_state *inputState = InitInputState();
    
    appFunctions.initApp(&platFunctions, rb);
    
    while(running)
    {
        SDLProcessEvents(inputState);
        
        renderFunctions.startFrame(rb);
        
        appFunctions.updateAndRender(rb, inputState);
        
        renderFunctions.endFrame(window, rb);
    }
    
    // TODO(ajeej): renderer function clean up
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}