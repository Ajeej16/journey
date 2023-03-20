
#include <windows.h>

#include <gl/gl.h>
#include "glcorearb.h"
#include "wglext.h"

#include "joy_utils.h"

global PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
global PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
global PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
global PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;

#define STRINGIFY(x) #x
#define TOKENPASTE(x, y) STRINGIFY(x ## y)

#define GLProc(type, name)\
global PFNGL##type##PROC name;
#include "opengl_functions.inc"

#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_gl.h"
#include "win32_joy_renderer.h"

//#include "joy_renderer.c"
#include "joy_gl.c"

#define CstrEqualLength(s0, s1, l) (CstrCmpLength((s0), (s1), (l)) == 0)

internal int
CstrCmpLength(const char *str0, const char *str1, u32 length)
{
    while (*str0 && (*str0 == *str1) && length--) { str0++; str1++; }
    return *(const u8 *)str0 - *(const u8 *)str1;
}

internal void APIENTRY
Win32DebugCallback(GLenum source, GLenum type, GLuint id,
                   GLenum severity, GLsizei length,
                   const GLchar *message, const void *user)
{
    return;
}

internal void
Win32InitWGL()
{
    // TODO(ajeej): error handling
    HWND dummy = CreateWindowExA(0, "STATIC", "DummyWindow",
                                 WS_OVERLAPPED,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 0, 0, 0, 0);
    
    HDC dummyDC = GetDC(dummy);
    
    PIXELFORMATDESCRIPTOR pixelDesc = {
        .nSize = sizeof(pixelDesc),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
    };
    
    int format = ChoosePixelFormat(dummyDC, &pixelDesc);
    if (!format)
    {
        // TODO(ajeej): 
        return;
    }
    
    if (!DescribePixelFormat(dummyDC, format, 
                             sizeof(pixelDesc), &pixelDesc))
    {
        // TODO(ajeej): error
        return;
    }
    if (!SetPixelFormat(dummyDC, format, &pixelDesc))
    {
        // TODO(ajeej): error
        return;
    }
    
    HGLRC rCtx = wglCreateContext(dummyDC);
    if (!rCtx)
    {
        // TODO(ajeej): error
        return;
    }
    
    if (!wglMakeCurrent(dummyDC, rCtx))
    {
        // TODO(ajeej): error
        return;
    }
    
    wglGetExtensionsStringARB = (void *)wglGetProcAddress("wglGetExtensionsStringARB");
    
    if (!wglGetExtensionsStringARB)
    {
        // TODO(ajeej): error
        return;
    }
    
    const char *exts = wglGetExtensionsStringARB(dummyDC);
    if (!exts)
    {
        // TODO(ajeej): error
        return;
    }
    
    
    const char *start = exts;
    for (;*exts != 0; start = exts)
    {
        while (*exts != '\0' && *exts != ' ')
            exts++;
        
        u32 length = (exts-start)-1;
        if (CstrEqualLength("WGL_ARB_create_context", start, length))
        {
            wglCreateContextAttribsARB = (void *)wglGetProcAddress("wglCreateContextAttribsARB");
        }
        else if (CstrEqualLength("WGL_ARB_pixel_format", start, length))
        {
            wglChoosePixelFormatARB = (void *)wglGetProcAddress("wglChoosePixelFormatARB");
        }
        else if (CstrEqualLength("WGL_EXT_swap_control", start, length))
        {
            wglSwapIntervalEXT = (void *)wglGetProcAddress("wglSwapIntervalEXT");
        }
        
        exts++;
    }
    
    if(!wglChoosePixelFormatARB || !wglCreateContextAttribsARB ||
       !wglSwapIntervalEXT)
    {
        // TODO(ajeej): error
        return;
    }
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(rCtx);
    ReleaseDC(dummy, dummyDC);
    DestroyWindow(dummy);
}

internal void
Win32SetPixelFormat(HDC dc)
{
    i32 attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0,
    };
    
    int format = 0;
    UINT formats = 0;
    if (!wglChoosePixelFormatARB(dc, attribs, NULL, 1,
                                 &format, &formats))
    {
        // TODO(ajeej): error
        return;
    }
    
    PIXELFORMATDESCRIPTOR pixelDesc = { .nSize = sizeof(pixelDesc) };
    if (!DescribePixelFormat(dc, format, sizeof(pixelDesc), &pixelDesc))
    {
        // TODO(ajeej): error
        return;
    }
    
    if (!SetPixelFormat(dc, format, &pixelDesc))
    {
        // TODO(ajeej): error
        return;
    }
}

internal void
Win32InitOpengl(HDC dc, gl_renderer *gl)
{
    Win32InitWGL();
    Win32SetPixelFormat(dc);
    
    i32 attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0,
    };
    
    HGLRC rCtx = wglCreateContextAttribsARB(dc, NULL, attribs);
    if (!rCtx)
    {
        // TODO(ajeej): error
        return;
    }
    
    if (! wglMakeCurrent(dc, rCtx))
    {
        // TODO(ajeej): error
        return;
    }
    
#define GLProc(type, name)\
gl->##name = (PFNGL##type##PROC)wglGetProcAddress(STRINGIFY(name));
#include "opengl_functions.inc"
    
    gl->glDebugMessageCallback(Win32DebugCallback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

INIT_RENDERER(InitRenderer)
{
    gl_renderer *gl = malloc(sizeof(gl_renderer));
    
    Win32InitOpengl(dc, gl);
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
    
    gl->glUseProgram(rb->shaders[0].id);
    
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
    SwapBuffers(wglGetCurrentDC());
}