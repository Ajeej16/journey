
// TODO(ajeej): find a way to not have this here
internal model_t
parse_obj(platform_functions *plat_funcs, asset_manager_t *assets, char *obj_dir, void *data,
          shader *shad);

inline model_t *
GetModel(asset_manager_t *manager, u64 model_id)
{
    return manager->models+model_id;
}

// TODO(ajeej): put this all together
internal u64
AddModel(platform_functions *plat_funcs, asset_manager_t *assets, char *filename,
         u64 shader_id)
{
    model_t *model = PushOnStack(&assets->models);
    
    file_info_t file = {0};
    char *path = get_dir(filename);
    void *data = NULL;
    
    plat_funcs->readFile(filename, &data, NULL);
    
    *model = parse_obj(plat_funcs, assets, path, data,
                       assets->shaders+shader_id);
    
    return assets->free_model_id++;
}

internal u64
AddMaterial(asset_manager_t *assets, char *name, u64 diffuse_tex_id, u64 specular_tex_id,
            color diffuse_color, color specular_color)
{
    material_t *material = PushOnStack(&assets->materials);
    
    material->name = strdup(name);
    material->maps[MATERIAL_MAP_DIFFUSE].tex_id = diffuse_tex_id;
    material->maps[MATERIAL_MAP_DIFFUSE].colour = diffuse_color;
    material->maps[MATERIAL_MAP_SPECULAR].tex_id = specular_tex_id;
    material->maps[MATERIAL_MAP_SPECULAR].colour = specular_color;
    
    return assets->free_material_id++;
}

internal u64
AddTexture(platform_functions *plat_funcs, asset_manager_t *assets, char *filename)
{
    asset_entry_t *entry = PushOnStack(&assets->entries);
    asset_texture_t *texture = NULL;
    
    file_info_t file = {0};
    plat_funcs->openFile(filename, &file);
    texture = malloc(sizeof(*texture)+file.size);
    texture->size = file.size;
    texture->data = texture+1;
    
    plat_funcs->readFileOfSize(&file, file.size, &texture->data);
    plat_funcs->closeFile(file);
    
    entry->type = ASSET_TEXTURE;
    entry->data = (void *)texture;
    
    
    return assets->free_texture_id++;
}

internal u64
AddTextureFromImage(asset_manager_t *assets, image_t image)
{
    asset_entry_t *entry = PushOnStack(&assets->entries);
    asset_raw_texture_t *texture = malloc(sizeof(*texture));
    
    memcpy(texture, &image, sizeof(*texture));
    
    entry->type = ASSET_RAW_TEXTURE;
    entry->data = (void *)texture;
    
    return assets->free_texture_id++;
}

internal void
InitAssetManager(asset_manager_t *assets)
{
    assets->models = NULL;
    assets->free_model_id = 0;
    assets->tex_ids = NULL;
    assets->free_texture_id = 0;
    assets->shaders = NULL;
    assets->free_shader_id = 0;
    assets->entries = NULL;
    
    image_t null_image = {0};
    null_image.data = malloc(1);
    *(u8 *)null_image.data = 255;
    null_image.width = 1;
    null_image.height = 1;
    null_image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    null_image.mipmaps = 1;
    
    AddTextureFromImage(assets, null_image);
    AddMaterial(assets, "NULL", 0, 0, COLOR(255, 255, 255, 255), COLOR(255, 255, 255, 255));
}

internal u64
AddShader(platform_functions *plat_funcs, asset_manager_t *assets, char *filename)
{
    asset_entry_t *entry = PushOnStack(&assets->entries);
    asset_shader_t *shad = NULL;
    
    entry->type = ASSET_SHADER;
    plat_funcs->readFile(filename, &shad, NULL);
    entry->data = (void *)shad;
    
    return assets->free_shader_id++;
}