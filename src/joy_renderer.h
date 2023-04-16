
#ifndef JOY_RENDERER_H
#define JOY_RENDERER_H

typedef struct color {
    u8 r, g, b, a;
} color;

#define COLOR(r, g, b, a) (color){ r, g, b, a }

enum {
    PRIMITIVE_POINTS = 0,
    PRIMITIVE_LINES,
    PRIMITIVE_LINE_LOOP,
    PRIMITIVE_LINE_STRIP,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN,
    PRIMITIVE_QUADS,
};

typedef enum uniform_type {
    UNIFORM_FLOAT = 0,
    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,
    UNIFORM_INT,
    UNIFORM_IVEC2,
    UNIFORM_IVEC3,
    UNIFORM_IVEC4,
    UNIFORM_MATRIX,
    UNIFORM_SAMPLER2D
} uniform_type;

enum {
    SHADER_LOC_MATRIX_VIEW = 0,
    SHADER_LOC_MATRIX_PROJECTION,
    SHADER_LOC_COUNT
};

#define SHADER_MATRIX_VIEW_NAME "view"
#define SHADER_MATRIX_PROJECTION_NAME "proj"

typedef struct uniform_entry {
    uniform_type type;
    void *data;
    u32 count;
} uniform_entry;

typedef struct shader_entry {
    u8 *code;
} shader_entry;

typedef struct shader {
    u32 id;
    i32 locs[SHADER_LOC_COUNT];
} shader;

typedef struct camera {
    v3 pos, front, up;
    f32 fov, sensitivity;
    i32 width, height;
    m4 view, projection;
} camera;

typedef struct render_cmd {
    u32 textureID;
    u32 indicesIdx;
    u32 indicesCount;
    u32 primitiveType;
} render_cmd;

typedef struct render_buffer {
    STACK(v3 *) vertices;
    STACK(v2 *) uvs;
    STACK(color *) colors;
    STACK(u16 *) indices;
    
    render_cmd *cmds;
    
    shader_entry shaderEntries[32];
    u32 shaderEntryCount;
    
    shader shaders[32];
    u32 shaderCount;
    
    camera cam;
} render_buffer;

#endif //JOY_RENDERER_H
