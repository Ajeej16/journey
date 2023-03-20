
#include <SDL2/SDL.h>

#include <gl/gl.h>
#include "glcorearb.h"

#include "joy_utils.h"

#define GLProc(type, name)\
global PFNGL##type##PROC name;
#include "opengl_functions.inc"

#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_gl.h"
#include "sdl_joy_renderer.h"

//#include "joy_renderer.c"
#include "joy_gl.c"

internal void
SDLInitOpengl(SDL_Window *window, gl_renderer *gl)
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    if(context == NULL)
    {
        // TODO(ajeej): error
        return
    }
    
#define GLProc(type, name)\
gl->##name = (PFNGL##type##PROC)SDL_GL_GetProcAddress(STRINGIFY(name));
#include "opengl_functions.inc"
}

INIT_RENDERER(InitRenderer)
{
    gl_renderer *gl = malloc(sizeof(gl_renderer));
    
    
    SDLInitOpengl(window, gl);
    InitGLRenderer(gl);
    
    glViewport(winPos.x, winPos.y, winDim.x, winDim.y);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_SMOOTH);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    
    return (render_buffer *)gl;
}

START_FRAME(StartFrame)
{
    gl_renderer *gl = (gl_renderer *)rb;
    
    glClearColor(255, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    for(u32 entry = 0;
        entry < rb->shaderEntryCount;
        entry++)
    {
        shader_entry *shaderEntry = rb->shaderEntries + entry;
        shader *newShader = rb->shaders + rb->shaderCount++;
        LoadShader(gl, newShader, shaderEntry->code);
        free(shaderEntry->code);
    }
    
    rb->shaderEntryCount = 0;
    
    SetUniform(gl, rb->shaders[0].locs[SHADER_LOC_MATRIX_VIEW],
               rb->cam.view.elements, UNIFORM_MATRIX, 1);
    SetUniform(gl, rb->shaders[0].locs[SHADER_LOC_MATRIX_PROJECTION],
               rb->cam.projection.elements, UNIFORM_MATRIX, 1);
}

END_FRAME(EndFrame)
{
    gl_renderer *gl = (gl_renderer *)rb;
    
    SubmitRenderBuffer(gl);
    
    ClearStack(rb->vertices);
    ClearStack(rb->uvs);
    ClearStack(rb->colors);
    ClearStack(rb->indices);
    ClearStack(rb->cmds);
    
    gl->glUseProgram(0);
    
    glFlush();
    SDL_GL_SwapWindow(window)
}

FREE_RENDERER(FreeRenderer)
{
    SDL_GLContext ctx = SDL_GL_GetCurrentContext();
    
    if(ctx)
        SDL_GL_DeleteContext();
    else
    {
        // TODO(ajeej): error
    }
}