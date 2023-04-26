
#ifndef JOY_ASSETS_H
#define JOY_ASSETS_H

typedef enum asset_type_t {
    ASSET_RAW_TEXTURE = 0,
    ASSET_TEXTURE,
    ASSET_SHADER,
} asset_type_t;

typedef image_t asset_raw_texture_t;

typedef struct asset_texture_t {
    u64 size;
    void *data;
} asset_texture_t;

typedef u8 asset_shader_t;

typedef struct asset_entry_t {
    asset_type_t type;
    void *data;
} asset_entry_t;

typedef struct asset_manager_t {
    STACK(model_t) *models;
    u32 free_model_id;
    
    STACK(material_t) *materials;
    u32 free_material_id;
    
    STACK(u64) *tex_ids;
    u32 free_texture_id;
    
    STACK(shader) *shaders;
    u32 free_shader_id;
    
    STACK(asset_entry_t) *entries;
} asset_manager_t;


#endif //JOY_ASSETS_H
