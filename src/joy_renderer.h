
#ifndef JOY_RENDERER_H
#define JOY_RENDERER_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    SHADER_LOC_MATRIX_MODEL,
    SHADER_LOC_SAMPLER2D_DIFFUSE,
    SHADER_LOC_SAMPLER2D_SPECULAR,
    SHADER_LOC_SAMPLER2D_NORMAL,
    SHADER_LOC_SAMPLER2D_OCCULSION,
    SHADER_LOC_VEC3_LIGHT_DIR,
    SHADER_LOC_VEC3_AMBIENT,
    SHADER_LOC_VEC3_DIFFUSE,
    SHADER_LOC_VEC3_SPECULAR,
    SHADER_LOC_VEC3_VIEW_POS,
    SHADER_LOC_COUNT
};

#define SHADER_MATRIX_VIEW_NAME "view"
#define SHADER_MATRIX_PROJECTION_NAME "proj"
#define SHADER_MATRIX_MODEL_NAME "model"
#define SHADER_SAMPLER2D_DIFFUSE_NAME "diffuse_map"
#define SHADER_SAMPLER2D_SPECULAR_NAME "specular_map"
#define SHADER_SAMPLER2D_NORMAL_NAME "normal_map"
#define SHADER_SAMPLER2D_OCCULSION_NAME "occulsion_map"
#define SHADER_VEC3_LIGHT_DIR_NAME "light.dir"
#define SHADER_VEC3_AMBIENT_NAME "light.ambient"
#define SHADER_VEC3_DIFFUSE_NAME "light.diffuse"
#define SHADER_VEC3_SPECULAR_NAME "light.specular"
#define SHADER_VEC3_VIEW_POS_NAME "view_pos"

global char *shader_var_names[] = {
    SHADER_MATRIX_VIEW_NAME,
    SHADER_MATRIX_PROJECTION_NAME, 
    SHADER_MATRIX_MODEL_NAME,
    SHADER_SAMPLER2D_DIFFUSE_NAME ,
    SHADER_SAMPLER2D_SPECULAR_NAME,
    SHADER_SAMPLER2D_NORMAL_NAME, 
    SHADER_SAMPLER2D_OCCULSION_NAME,
    SHADER_VEC3_LIGHT_DIR_NAME,
    SHADER_VEC3_AMBIENT_NAME,
    SHADER_VEC3_DIFFUSE_NAME,
    SHADER_VEC3_SPECULAR_NAME,
    SHADER_VEC3_VIEW_POS_NAME,
};


typedef struct uniform_entry {
    uniform_type type;
    void *data;
    u32 count;
} uniform_entry;

typedef struct shader {
    u32 id;
    i32 locs[SHADER_LOC_COUNT];
} shader;


typedef enum pixel_format_t {
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1,
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,
    PIXELFORMAT_UNCOMPRESSED_R5G6B5,
    PIXELFORMAT_UNCOMPRESSED_R8G8B8,
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    PIXELFORMAT_UNCOMPRESSED_R32,
    PIXELFORMAT_UNCOMPRESSED_R32G32B32,
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,
    PIXELFORMAT_COMPRESSED_DXT1_RGB,
    PIXELFORMAT_COMPRESSED_DXT1_RGBA,
    PIXELFORMAT_COMPRESSED_DXT3_RGBA,
    PIXELFORMAT_COMPRESSED_DXT5_RGBA,
    PIXELFORMAT_COMPRESSED_ETC1_RGB,
    PIXELFORMAT_COMPRESSED_ETC2_RGB,
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,
    PIXELFORMAT_COMPRESSED_PVRT_RGB,
    PIXELFORMAT_COMPRESSED_PVRT_RGBA,
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA
} pixel_format_t;

typedef struct texture_t {
    union{
        void *data;
        u64 id;
    };
    
    u32 width, height;
    u32 mipmaps;
    u32 format;
} texture_t;

typedef texture_t image_t;

typedef enum material_map_idx_t {
    MATERIAL_MAP_DIFFUSE = 0,
    MATERIAL_MAP_SPECULAR,
    MATERIAL_MAP_NORMAL,
    MATERIAL_MAP_ROUGHNESS,
    MATERIAL_MAP_OCCLUSION,
    MATERIAL_MAP_EMISSION,
    MATERIAL_MAP_HEIGHT,
    MATERIAL_MAP_CUBEMAP,
    MATERIAL_MAP_IRRADIANCE,
    MATERIAL_MAP_PREFILTER,
    MATERIAL_MAP_BRDF,
} material_map_idx_t;

typedef struct material_map_t {
    u64 tex_id;
    color colour;
    f32 value;
} material_map_t;

typedef struct material_t {
    char *name;
    material_map_t maps[5];
} material_t;

typedef struct mesh_t {
    u16 *indices;
    u32 material_id;
} mesh_t;

// TODO(ajeej): change verts to not be dynamic
typedef struct model_t {
    STACK(v3) *verts;
    STACK(v2) *tex_coords;
    STACK(v3) *norms;
    
    STACK(mesh_t) *meshes;
    
    m4 transform;
} model_t;


typedef struct camera {
    v3 pos, front, up;
    f32 fov, sensitivity;
    i32 width, height;
    m4 view, projection;
} camera;

typedef struct render_cmd {
    u32 materialID;
    m4 transform;
    u32 indicesIdx;
    u32 indicesCount;
    u32 primitiveType;
} render_cmd;

typedef struct render_buffer {
    STACK(v3 *) vertices;
    STACK(v2 *) uvs;
    STACK(v3 *) normals;
    STACK(color *) colors;
    STACK(u16 *) indices;
    
    render_cmd *cmds;
    
    camera cam;
} render_buffer;

#endif //JOY_RENDERER_H
