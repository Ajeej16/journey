#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "glcorearb.h"

#ifdef _WIN32
#include "GL/gl.h"
#elif __APPLE__
#include "Opengl/gl3.h"
#endif

#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_gl.h"
#include "glfw_joy_renderer.h"

//#include "joy_renderer.c"
#include "joy_gl.c"

internal void
GLFWInitOpengl(GLFWwindow *window, gl_renderer *gl)
{
    glfwMakeContextCurrent(window);

    // Load Opengl Functions
    {
        gl->glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glfwGetProcAddress("glBindAttribLocation");
        gl->glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
        gl->glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)glfwGetProcAddress("glBindBufferBase");
        gl->glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)glfwGetProcAddress("glBindBufferRange");
        gl->glBindBuffersBase = (PFNGLBINDBUFFERSBASEPROC)glfwGetProcAddress("glBindBuffersBase");
        gl->glBindBuffersRange = (PFNGLBINDBUFFERSRANGEPROC)glfwGetProcAddress("glBindBuffersRange");
        gl->glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
        gl->glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
        gl->glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)glfwGetProcAddress("glBindVertexBuffer");
        gl->glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
        gl->glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)glfwGetProcAddress("glNamedBufferData");
        gl->glBufferStorage = (PFNGLBUFFERSTORAGEPROC)glfwGetProcAddress("glBufferStorage");
        gl->glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)glfwGetProcAddress("glNamedBufferStorage");
        gl->glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glfwGetProcAddress("glBufferSubData");
        gl->glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)glfwGetProcAddress("glNamedBufferSubData");
        gl->glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
        gl->glCreateBuffers = (PFNGLCREATEBUFFERSPROC)glfwGetProcAddress("glCreateBuffers");
        gl->glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)glfwGetProcAddress("glCreateFramebuffers");
        gl->glCreateTextures = (PFNGLCREATETEXTURESPROC)glfwGetProcAddress("glCreateTextures");
        gl->glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)glfwGetProcAddress("glCreateVertexArrays");
        gl->glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");
        gl->glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
        gl->glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
        gl->glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
        gl->glDetachShader = (PFNGLDETACHSHADERPROC)glfwGetProcAddress("glDetachShader");
        gl->glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
        gl->glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
        gl->glMapBuffer = (PFNGLMAPBUFFERPROC)glfwGetProcAddress("glMapBuffer");
        gl->glMapNamedBuffer = (PFNGLMAPNAMEDBUFFERPROC)glfwGetProcAddress("glMapNamedBuffer");
        gl->glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)glfwGetProcAddress("glMapBufferRange");
        gl->glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)glfwGetProcAddress("glMapNamedBufferRange");
        gl->glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
        gl->glCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
        gl->glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
        gl->glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)glfwGetProcAddress("glCreateShaderProgramv");
        gl->glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)glfwGetProcAddress("glGetUniformBlockIndex");
        gl->glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
        gl->glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
        gl->glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
        gl->glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
        gl->glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
        gl->glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
        gl->glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
        gl->glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
        gl->glUniform1fv = (PFNGLUNIFORM1FVPROC)glfwGetProcAddress("glUniform1fv");
        gl->glUniform2fv = (PFNGLUNIFORM2FVPROC)glfwGetProcAddress("glUniform2fv");
        gl->glUniform3fv = (PFNGLUNIFORM3FVPROC)glfwGetProcAddress("glUniform3fv");
        gl->glUniform4fv = (PFNGLUNIFORM4FVPROC)glfwGetProcAddress("glUniform4fv");
        gl->glUniform1iv = (PFNGLUNIFORM1IVPROC)glfwGetProcAddress("glUniform1iv");
        gl->glUniform2iv = (PFNGLUNIFORM2IVPROC)glfwGetProcAddress("glUniform2iv");
        gl->glUniform3iv = (PFNGLUNIFORM3IVPROC)glfwGetProcAddress("glUniform3iv");
        gl->glUniform4iv = (PFNGLUNIFORM4IVPROC)glfwGetProcAddress("glUniform4iv");
        gl->glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv");
    }

    glfwSwapInterval(1);
}

INIT_RENDERER(InitRenderer)
{
    gl_renderer *gl = malloc(sizeof(gl_renderer));

    GLFWInitOpengl(window, gl);
    InitGLRenderer(gl);

    u32 w = 0, h = 0, x = 0, y = 0;
    glfwGetWindowSize(window, &w, &h);;
    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_SMOOTH);
    //glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    return (render_buffer *)gl;
}

START_FRAME(StartFrame)
{
    gl_renderer *gl = (gl_renderer *)rb;

    u32 w = 0, h = 0;
    glfwGetFramebufferSize(window, &w, &h);

    glViewport(0, 0, w, h);

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
    glfwSwapBuffers(window);
}
