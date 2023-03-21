
#include <SDL2/SDL.h>

#include <GL/gl.h>
#include "glcorearb.h"

#include "joy_utils.h"

#define STRINGIFY(x) #x
#define TOKENPASTE(x, y) STRINGIFY(x ## y)

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
    if(ctx == NULL)
    {
        // TODO(ajeej): error
        return;
    }
    
    gl->glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glBindAttribLocation");
    gl->glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
    gl->glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)SDL_GL_GetProcAddress("glBindBufferBase");
    gl->glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)SDL_GL_GetProcAddress("glBindBufferRange");
    gl->glBindBuffersBase = (PFNGLBINDBUFFERSBASEPROC)SDL_GL_GetProcAddress("glBindBuffersBase");
    gl->glBindBuffersRange = (PFNGLBINDBUFFERSRANGEPROC)SDL_GL_GetProcAddress("glBindBuffersRange");
    gl->glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)SDL_GL_GetProcAddress("glBindFramebuffer");
    gl->glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)SDL_GL_GetProcAddress("glBindVertexArray");
    gl->glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)SDL_GL_GetProcAddress("glBindVertexBuffer");
    gl->glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
    gl->glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)SDL_GL_GetProcAddress("glNamedBufferData");
    gl->glBufferStorage = (PFNGLBUFFERSTORAGEPROC)SDL_GL_GetProcAddress("glBufferStorage");
    gl->glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)SDL_GL_GetProcAddress("glNamedBufferStorage");
    gl->glBufferSubData = (PFNGLBUFFERSUBDATAPROC)SDL_GL_GetProcAddress("glBufferSubData");
    gl->glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)SDL_GL_GetProcAddress("glNamedBufferSubData");
    gl->glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
    gl->glCreateBuffers = (PFNGLCREATEBUFFERSPROC)SDL_GL_GetProcAddress("glCreateBuffers");
    gl->glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glCreateFramebuffers");
    gl->glCreateTextures = (PFNGLCREATETEXTURESPROC)SDL_GL_GetProcAddress("glCreateTextures");
    gl->glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glCreateVertexArrays");
    gl->glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
    gl->glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteFramebuffers");
    gl->glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
    gl->glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glDeleteVertexArrays");
    gl->glDetachShader = (PFNGLDETACHSHADERPROC)SDL_GL_GetProcAddress("glDetachShader");
    gl->glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
    gl->glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glGenVertexArrays");
    gl->glMapBuffer = (PFNGLMAPBUFFERPROC)SDL_GL_GetProcAddress("glMapBuffer");
    gl->glMapNamedBuffer = (PFNGLMAPNAMEDBUFFERPROC)SDL_GL_GetProcAddress("glMapNamedBuffer");
    gl->glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)SDL_GL_GetProcAddress("glMapBufferRange");
    gl->glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)SDL_GL_GetProcAddress("glMapNamedBufferRange");
    gl->glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
    gl->glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
    gl->glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
    gl->glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)SDL_GL_GetProcAddress("glCreateShaderProgramv");
    gl->glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)SDL_GL_GetProcAddress("glGetUniformBlockIndex");
    gl->glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
    gl->glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
    gl->glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glShaderSource");
    gl->glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
    gl->glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
    gl->glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
    gl->glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
    gl->glGetProgramiv = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");
    gl->glUniform1fv = (PFNGLUNIFORM1FVPROC)SDL_GL_GetProcAddress("glUniform1fv");
    gl->glUniform2fv = (PFNGLUNIFORM2FVPROC)SDL_GL_GetProcAddress("glUniform2fv");
    gl->glUniform3fv = (PFNGLUNIFORM3FVPROC)SDL_GL_GetProcAddress("glUniform3fv");
    gl->glUniform4fv = (PFNGLUNIFORM4FVPROC)SDL_GL_GetProcAddress("glUniform4fv");
    gl->glUniform1iv = (PFNGLUNIFORM1IVPROC)SDL_GL_GetProcAddress("glUniform1iv");
    gl->glUniform2iv = (PFNGLUNIFORM2IVPROC)SDL_GL_GetProcAddress("glUniform2iv");
    gl->glUniform3iv = (PFNGLUNIFORM3IVPROC)SDL_GL_GetProcAddress("glUniform3iv");
    gl->glUniform4iv = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniform4iv");

}

INIT_RENDERER(InitRenderer)
{
    gl_renderer *gl = malloc(sizeof(gl_renderer));
    
    
    SDLInitOpengl(window, gl);
    InitGLRenderer(gl);
    
    u32 w = 0, h = 0, x = 0, y = 0;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GetWindowPosition(window, &x, &y);
    glViewport(x, y, w, h);
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
    SDL_GL_SwapWindow(window);
}

FREE_RENDERER(FreeRenderer)
{
    SDL_GLContext ctx = SDL_GL_GetCurrentContext();
    
    if(ctx)
        SDL_GL_DeleteContext(ctx);
    else
    {
        // TODO(ajeej): error
    }
}
