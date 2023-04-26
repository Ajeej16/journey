
#ifndef JOY_GL_H
#define JOY_GL_H

#define GLProc(type, name) PFNGL##type##PROC name;
typedef struct gl_renderer {
    render_buffer rb;
    
    u32 vaoID;
    u32 *vboID;
    
#include "opengl_functions.inc"
} gl_renderer;

enum {
    VBO_POSITION = 0,
    VBO_UV,
    VBO_NORMAL,
    VBO_COLOR,
    VBO_INDEX,
    VBO_COUNT,
};

#endif //JOY_GL_H
