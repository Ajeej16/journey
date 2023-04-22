

typedef struct vert_index_t {
    u32 v_idx, t_idx, n_idx;
} vert_index_t;

enum {
    VERT_TYPE_NUL = 0,
    VERT_TYPE_POS = 1,
    VERT_TYPE_TEX = 2,
    VERT_TYPE_NOR = 3,
};

internal char *
parse_f_command(char *token, vert_index_t *vi)
{
    vi->v_idx = (u32)(-1); vi->t_idx = (u32)(-1); vi->n_idx = (u32)(-1);
    
    vi->v_idx = strtol(token, &token, 10);
    if(*token != '/')
        return token;
    token++;
    
    if(*token == '/') {
        token++;
        vi->n_idx = strtol(token, &token, 10);
        return token;
    }
    
    vi->t_idx = strtol(token, &token, 10);
    if(*token != '/')
        return token;
    token++;
    
    vi->n_idx = strtol(token, &token, 10);
    return token;
}

internal char *
parse_vec3(char *token, f32 el[3])
{
    for(u32 i = 0; i < 3; i++)
    {
        el[i] = strtof(token, &token);
        token = skip_spaces_and_cr(token);
    }
    
    return token;
}

internal char *
parse_v(char *token, v3 **v, v2 **t, v3 **n)
{
    u32 vert_type = 0;
    f32 *el = NULL;
    u32 el_count = 0;
    u32 el_idx;
    
    while(*token == 'v')
    {
        token ++;
        el_idx = 0;
        
        if(*token == ' ') { 
            vert_type = VERT_TYPE_POS;
            el = (f32 *)PushOnStack(v);
            el_count = 3;
            token += 1;
        }
        else if (*token == 't') {
            vert_type = VERT_TYPE_TEX;
            el = (f32 *)PushOnStack(t);
            el_count = 2;
            token += 2;
        }
        else if (*token == 'n') {
            vert_type = VERT_TYPE_NOR;
            el = (f32 *)PushOnStack(n);
            el_count = 3;
            token += 2;
        }
        else
            ASSERT(0);
        
        while(*token != '\n') {
            
            ASSERT(el_idx < el_count);
            
            el[el_idx++] = strtof(token, &token);
            token = skip_spaces_and_cr(token);
        }
        token++;
    }
    
    return token;
}

internal char *
parse_f(char *token, v3 *v, v2 *t, v3 *n, model_t *model,
        u32 *vert_idx, u32 *indices_idx, hash_table_t **idx_hash,
        u32 v_offset, u32 t_offset, u32 n_offset)
{
    u32 vert_count;
    u16 vert_indices[16];
    vert_index_t vi = {0};
    v3 *nv = NULL, *nn = NULL;
    v2 *nt = NULL;
    char *end = NULL;
    char *key = NULL;
    u16 *hashed_idx = NULL;
    u16 *indices = NULL;
    mesh_t *cur_mesh = GetStackLast(model->meshes);
    
    while(*token == 'f') 
    {
        token += 2;
        vert_count = 0;
        
        while(*token != '\n')
        {
            end = seek_chars(token, " \n", 2);
            key = malloc(end-token+1);
            memcpy(key, token, end-token);
            key[end-token] = 0;
            if(hashed_idx = (u16 *)get_hash_entry(*idx_hash, key))
            {
                vert_indices[vert_count++] = *hashed_idx;
                free(key);
                token = skip_spaces_and_cr(end);
                continue;
            }
            
            if(!set_hash_entry(idx_hash, key, indices_idx))
                ASSERT(0);
            free(key);
            
            token = skip_spaces_and_cr(parse_f_command(token, &vi));
            if(vi.v_idx > 492)
            {
                int x = 0;
                x = 3;
            }
            
            v3 *tempv = PushOnStack(&model->verts);
            *tempv = v[vi.v_idx-1-v_offset];
            v2 *tempt = PushOnStack(&model->tex_coords);
            *tempt = t[vi.t_idx-1-t_offset];
            v3 *tempn = PushOnStack(&model->norms);
            *tempn = v[vi.n_idx-1-n_offset];
            vert_indices[vert_count++] = (*indices_idx)++;
        }
        token++;
        
        u32 idx, i = 0;
        u32 face_count = 0;
        indices = PushArrayOnStack(&cur_mesh->indices, 3*vert_count-6);
        u16 i0 = vert_indices[0], i1, i2 = vert_indices[1];
        for(idx = 2; idx < vert_count; idx++, i++)
        {
            i1 = i2;
            i2 = vert_indices[idx];
            
            ASSERT(3*i+2 < 3*vert_count-6);
            
            indices[3*i] = i0;
            indices[3*i+1] = i1;
            indices[3*i+2] = i2;
            
            face_count++;
        }
    }
    
    return token;
}

internal char *
parse_command_filename(char *token, char **name)
{
    char *end = seek_chars(token, " \n", 2);
    u32 size = end-token;
    
    *name = malloc(size+1);
    memcpy(*name, token, size);
    (*name)[size] = 0;
    
    return end;
}

internal void
parse_mtl(platform_functions *plat_funcs, asset_manager_t *assets, char *mtl_dir,
          u8 *token, material_t **materials, shader *shad)
{
    material_t *material = 0;
    while(*token)
    {
        if(strncmp(token, "newmtl", 6) == 0)
        {
            material = PushOnStack(materials);
            *material = load_default_material(shad);
            
            token += 7;
            
            token = skip_spaces_and_cr(parse_command_filename(token, &material->name));
            token++;
        }
        // TODO(ajeej): make into one branch with k's
        else if(*token == 'K')
        {
            token += 1;
            u32 idx = 0;
            if(*token == 'a')
                idx = MATERIAL_MAP_OCCLUSION;
            else if(*token == 'd')
                idx = MATERIAL_MAP_DIFFUSE;
            else if(*token == 's')
                idx = MATERIAL_MAP_SPECULAR;
            else {
                // TODO(ajeej): add rest
                token = seek_char(token, '\n');
                token++;
                continue;
            }
            
            token += 2;
            f32 el[3];
            token = parse_vec3(token, el);
            
            material->maps[idx].color = COLOR(el[0]*255,
                                              el[1]*255,
                                              el[2]*255,
                                              255);
            token++;
        }
        else if(strncmp(token, "map_", 4) == 0)
        {
            token += 4;
            u32 idx = 0;
            if(*token == 'K') {
                token += 1;
                if(*token == 'a')
                    idx = MATERIAL_MAP_OCCLUSION;
                else if(*token == 'd')
                    idx = MATERIAL_MAP_DIFFUSE;
                else if(*token == 's')
                    idx = MATERIAL_MAP_SPECULAR;
                else {
                    // TODO(ajeej): add rest
                    token = seek_char(token, '\n');
                    token++;
                    continue;
                }
                token += 2;
            }
            else if(*token == 'B') {
                idx = MATERIAL_MAP_NORMAL;
                token += 5;
            }
            else
            {
                // TODO(ajeej): add rest
                token = seek_char(token, '\n');
                token++;
                continue;
            }
            
            char *filename = NULL;
            token = skip_spaces_and_cr(parse_command_filename(token, &filename));
            
            char *path = malloc(strlen(filename)+strlen(mtl_dir)+2);
            memset(path, 0, strlen(filename)+strlen(mtl_dir)+2);
            CstrCatMany(path, mtl_dir, "\\", filename);
            material->maps[idx].tex_id = AddTexture(plat_funcs, assets, path);
            free(filename);
            free(path);
            
            token++;
        }
        else
        {
            token = seek_char(token, '\n');
            token++;
        }
    }
}

// TODO(ajeej): pass obj file path to function, need in order to load mtl and textures
//              take the directory and attach mtl/texture filename to the end

internal model_t
parse_obj(platform_functions *plat_funcs, asset_manager_t *assets, char *obj_dir, void *data,
          shader *shad)
{
    char *token = (char *)data;
    
    model_t model = {0};
    mesh_t *cur_mesh = NULL;
    u32 vert_idx = 0;
    u32 indices_idx = 0;
    
    v3 *v = NULL;
    v2 *t = NULL;
    v3 *n = NULL;
    u32 v_offset = 0;
    u32 t_offset = 0;
    u32 n_offset = 0;
    
    hash_table_t *idx_hash = create_hash_table_of_size(16, sizeof(u16));
    
    while(*token)
    {
        if(*token == 'v')
        {
            v_offset += GetStackCount(v);
            ClearStack(v);
            t_offset += GetStackCount(t);
            ClearStack(t);
            n_offset += GetStackCount(n);
            ClearStack(n);
            token = parse_v(token, &v, &t, &n);
        }
        else if(*token == 'o')
        {
            cur_mesh = PushOnStack(&model.meshes);
            PushArrayOnStack(&cur_mesh->indices, 200);
            ClearStack(cur_mesh->indices);
            token++;
        }
        else if(*token == 'f')
        {
            ASSERT(v || t || n);
            
            if(!cur_mesh)
                cur_mesh = PushOnStack(&model.meshes);
            
            token = parse_f(token, v, t, n, &model,
                            &vert_idx, &indices_idx,
                            &idx_hash,
                            v_offset, t_offset, n_offset);
            
        }
        else if(strncmp(token, "mtllib", 6) == 0)
        {
            token += 7;
            
            char *mtl_name = NULL; 
            token = skip_spaces_and_cr(parse_command_filename(token, &mtl_name));
            
            char *path = malloc(strlen(mtl_name)+strlen(obj_dir)+2);
            memset(path, 0, strlen(mtl_name)+strlen(obj_dir)+2);
            CstrCatMany(path, obj_dir, "\\", mtl_name);
            
            void *data = NULL;
            plat_funcs->readFile(path, &data, NULL);
            parse_mtl(plat_funcs, assets, obj_dir, (u8 *)data, &assets->materials,
                      shad);
            free(mtl_name);
            free(path);
            
            token++;
        }
        else if(strncmp(token, "usemtl", 6) == 0)
        {
            token += 7;
            
            char *mat_name = NULL;
            token = skip_spaces_and_cr(parse_command_filename(token, &mat_name));
            
            material_t *m = NULL;
            for(u32 i = 0; i < GetStackCount(assets->materials); i++)
            {
                m = assets->materials+i;
                if(strcmp(mat_name, m->name) == 0)
                    cur_mesh->material_id = i;
            }
            
            free(mat_name);
        }
        else
        {
            token = seek_char(token, '\n');
            token++;
        }
    }
    
    free_hash_table(idx_hash);
    FreeStack(v);
    FreeStack(t);
    FreeStack(n);
    
    return model;
}