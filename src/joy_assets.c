
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
}

// TODO(ajeej): put this all together
internal u64
AddModel(platform_functions *plat_funcs, asset_manager_t *assets, char *filename,
         u64 shader_id)
{
    asset_entry_t *entry = PushOnStack(&assets->entries);
    asset_model_t *model = NULL;
    
    file_info_t file = {0};
    plat_funcs->openFile(filename, &file);
    model = malloc(sizeof(*model)+file.size);
    model->path = get_dir(filename);
    model->shader_id = shader_id;
    model->data = model+1;
    
    plat_funcs->readFileOfSize(&file, file.size, &model->data);
    plat_funcs->closeFile(file);
    
    entry->type = ASSET_MODEL;
    entry->data = (void *)model;
    
    return assets->free_model_id++;
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
AddShader(platform_functions *plat_funcs, asset_manager_t *assets, char *filename)
{
    asset_entry_t *entry = PushOnStack(&assets->entries);
    asset_shader_t *shad = NULL;
    
    entry->type = ASSET_SHADER;
    plat_funcs->readFile(filename, &shad, NULL);
    entry->data = (void *)shad;
    
    return assets->free_shader_id++;
}