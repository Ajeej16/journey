
internal void
ResizeCamera(camera *c, i32 width, i32 height)
{
    if(c->width == width && c->height == height)
        return;
    
    // TODO(ajeej): variables for near and far plane
    c->projection = get_perspective(c->fov, ((f32)width/(f32)height), 0.01f, 1000.0f);
    c->width = width;
    c->height = height;
}

internal void
InitCamera(camera *c, v3 pos, f32 sensitivity, 
           f32 fov, i32 width, i32 height)
{
    c->pos = pos;
    
    c->front = vec3_init(0.0f, 0.0f, -1.0f);
    c->up = vec3_init(0.0f, 1.0f, 0.0f);
    c->fov = fov;
    c->sensitivity = sensitivity;
    c->width = 0;
    c->height = 0;
    
    c->view = mat4_identity();
    c->projection = mat4_identity();
    
    ResizeCamera(c, width, height);
}

internal void
UpdateCamera(camera *c)
{
    v3 center = vec3_add(c->pos, c->front);
    c->view = get_look_at(c->pos, center, c->up);
}

internal u32
GetPixelDataSize(u32 width, u32 height, u32 format)
{
    u32 data_size = 0;
    u32 bytes_per_pixel = 0;
    
    switch(format)
    {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: bytes_per_pixel = 8; break;
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
        case PIXELFORMAT_UNCOMPRESSED_R5G6B5:
        case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
        case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: bytes_per_pixel = 16; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
        case PIXELFORMAT_UNCOMPRESSED_R32: bytes_per_pixel = 32; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8: bytes_per_pixel = 24; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32: bytes_per_pixel = 96; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: bytes_per_pixel = 128; break;
        case PIXELFORMAT_COMPRESSED_DXT1_RGB:
        case PIXELFORMAT_COMPRESSED_DXT1_RGBA:
        case PIXELFORMAT_COMPRESSED_ETC1_RGB:
        case PIXELFORMAT_COMPRESSED_ETC2_RGB:
        case PIXELFORMAT_COMPRESSED_PVRT_RGB:
        case PIXELFORMAT_COMPRESSED_PVRT_RGBA: bytes_per_pixel = 4; break;
        case PIXELFORMAT_COMPRESSED_DXT3_RGBA:
        case PIXELFORMAT_COMPRESSED_DXT5_RGBA:
        case PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA:
        case PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA: bytes_per_pixel = 8; break;
        case PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA: bytes_per_pixel = 2; break;
        default: ASSERT(0); break;
    }
    
    data_size = width*height*bytes_per_pixel/8;
    
    if(width < 4 && height < 4)
    {
        if(format >= PIXELFORMAT_COMPRESSED_DXT1_RGB && format < PIXELFORMAT_COMPRESSED_DXT3_RGBA)
            data_size = 8;
        else if(format >= PIXELFORMAT_COMPRESSED_DXT3_RGBA && format < PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA)
            data_size = 16;
    }
    
    return data_size;
}

internal image_t
LoadImageFromMemory(u8 *data, u64 size)
{
    image_t image = {0};
    
    u32 comp = 0;
    image.data = stbi_load_from_memory(data, size,
                                       &image.width, &image.height,
                                       &comp, 0);
    
    ASSERT(image.data);
    image.mipmaps = 1;
    if(comp == 1) image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    else if(comp == 2) image.format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
    else if(comp == 3) image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    else if(comp == 4) image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    else ASSERT(0);
    
    return image;
}

internal image_t
LoadImageFromFile(platform_read_file *read_file, char *filename)
{
    image_t image = {0};
    
    u64 size = 0;
    u8 *data = 0;
    read_file(filename, (void **)&data, &size);
    
    if(!data)
        ASSERT(0);
    image = LoadImageFromMemory(data, size);
}

internal void
UnloadImage(image_t image)
{
    stbi_image_free(image.data);
}

internal material_t
load_default_material()
{
    material_t material = {0};
    
    material.maps[MATERIAL_MAP_DIFFUSE].tex_id = (u32)(-1);
    material.maps[MATERIAL_MAP_DIFFUSE].colour = COLOR(255, 255, 255, 255);
    material.maps[MATERIAL_MAP_SPECULAR].colour = COLOR(255, 255, 255, 255);
    return material;
}

internal render_cmd *
PushRenderCommand(render_buffer *rb, u32 materialID, m4 transform,
                  u32 indicesCount, u32 primitiveType)
{
    render_cmd *rc = GetStackLast(rb->cmds);
    if(1 ||!rc || rc->primitiveType != primitiveType || rc->materialID != materialID)
    {
        rc = PushOnStack(&rb->cmds);
        rc->indicesIdx = GetStackCount(rb->indices);
        rc->indicesCount = 0;
        rc->materialID = materialID;
        rc->transform = transform;
        rc->primitiveType = primitiveType;
    }
    
    rc->indicesCount += indicesCount;
    return rc;
}

internal void
QuadToVerts(v3 *verts, v3 pos, v2 dim)
{
    
    verts[0] = pos;
    verts[1] = vec3_init(pos.x, pos.y+dim.y, pos.z);
    verts[2] = vec3_init(pos.x+dim.x, pos.y+dim.y, pos.z);
    verts[3] = vec3_init(pos.x+dim.x, pos.y, pos.z);
}


// TODO(ajeej): have work with material id
/*internal void
PushTexturedQuad(render_buffer *rb, v3 pos, v2 dim, v2 uv[4],
                 u32 materialID, color tint)
{
    v3 quadVerts[4];
    QuadToVerts(quadVerts, pos, dim);
    
    PushRenderCommand(rb, materialID, 6, PRIMITIVE_TRIANGLES);
    
    u32 vertIdx = GetStackCount(rb->vertices);
    
    v3 *verts = PushArrayOnStack(&rb->vertices, 4);
    v2 *uvs = PushArrayOnStack(&rb->uvs, 4);
    color *colors = PushArrayOnStack(&rb->colors, 4);
    u16 *indices = PushArrayOnStack(&rb->indices, 6);
    
    for (u32 i = 0; i < 4; i++)
    {
        verts[i] = quadVerts[i];
        uvs[i] = uv[i];
        colors[i] = tint;
    }
    
    indices[0] = vertIdx;
    indices[1] = vertIdx + 1;
    indices[2] = vertIdx + 2;
    indices[3] = vertIdx;
    indices[4] = vertIdx + 2;
    indices[5] = vertIdx + 3;
}

inline internal void
PushQuad(render_buffer *rb, v3 pos, v2 dim, color tint)
{
    v2 uv[4] = { 
        vec2_init(0.0f, 1.0f), vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f), vec2_init(1.0f, 1.0f) 
    };
    
    // TODO(ajeej): pass white texture id
    PushTexturedQuad(rb, pos, dim, uv, 0, tint);
}*/

/*internal void
PushCube(render_buffer *rb, v3 pos, v3 dim, color tint)
{
    v3 cubeVerts[8];
    f32 halfX = dim.x*0.5f, halfY = dim.y*0.5f, halfZ = dim.z*0.5f;
    cubeVerts[0] = vec3_init(pos.x-halfX, pos.y+halfY, pos.z+halfZ);
    cubeVerts[1] = vec3_init(pos.x+halfX, pos.y+halfY, pos.z+halfZ);
    cubeVerts[2] = vec3_init(pos.x+halfX, pos.y+halfY, pos.z-halfZ);
    cubeVerts[3] = vec3_init(pos.x-halfX, pos.y+halfY, pos.z-halfZ);
    cubeVerts[4] = vec3_init(pos.x-halfX, pos.y-halfY, pos.z+halfZ);
    cubeVerts[5] = vec3_init(pos.x+halfX, pos.y-halfY, pos.z+halfZ);
    cubeVerts[6] = vec3_init(pos.x+halfX, pos.y-halfY, pos.z-halfZ);
    cubeVerts[7] = vec3_init(pos.x-halfX, pos.y-halfY, pos.z-halfZ);
    
    v2 uv[8] = { 
        vec2_init(0.0f, 1.0f), vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f), vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 1.0f), vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f), vec2_init(1.0f, 1.0f),
    };
    
    PushRenderCommand(rb, 0, 36, PRIMITIVE_TRIANGLES);
    
    u32 vertIdx = GetStackCount(rb->vertices);
    
    v3 *verts = PushArrayOnStack(&rb->vertices, 8);
    v2 *uvs = PushArrayOnStack(&rb->uvs, 8);
    color *colors = PushArrayOnStack(&rb->colors, 8);
    u16 *indices = PushArrayOnStack(&rb->indices, 36);
    
    for (u32 i = 0; i < 8; i++)
    {
        verts[i] = cubeVerts[i];
        uvs[i] = uv[i];
        colors[i] = tint;
    }
    
    u32 sideStart = vertIdx;
    for(u32 side_idx = 0; side_idx < 4; side_idx++, sideStart++) {
        indices[side_idx*6] = sideStart;
        indices[side_idx*6+1] = sideStart+4;
        indices[side_idx*6+2] = (sideStart-vertIdx+5 == 8) ? vertIdx+4 : sideStart+5;
        
        indices[side_idx*6+3] = sideStart;
        indices[side_idx*6+4] = (sideStart-vertIdx+5 == 8) ? vertIdx+4 : sideStart+5;
        indices[side_idx*6+5] = (sideStart-vertIdx+5 == 8) ? vertIdx : sideStart+1;
    }
    
    u32 baseStart = vertIdx;
    for(u32 base_idx = 4; base_idx < 6; base_idx++, baseStart += 4) {
        indices[base_idx*6] = baseStart;
        indices[base_idx*6+1] = baseStart+1;
        indices[base_idx*6+2] = baseStart+3;
        
        indices[base_idx*6+3] = baseStart+1;
        indices[base_idx*6+4] = baseStart+2;
        indices[base_idx*6+5] = baseStart+3;
    }
    
}*/

inline internal void
UpdateMaterial(asset_manager_t *assets, model_t *model, u64 material_id)
{
    // TODO(ajeej): be able to choose which mesh gets which material
    for(u32 m_idx = 0; m_idx < GetStackCount(model->meshes); m_idx++)
        model->meshes[m_idx].material_id = material_id;
}

internal u64
CreateModel(asset_manager_t *assets, v3 *verts, v2 *uvs, v3 *norms, 
            u64 vert_count, u16 *indices, u64 idx_count)
{
    model_t *model = PushOnStack(&assets->models);
    model->transform = mat4_identity();
    
    v3 *m_verts = PushArrayOnStack(&model->verts, vert_count);
    v2 *m_uvs = PushArrayOnStack(&model->tex_coords, vert_count);
    v3 *m_norms = PushArrayOnStack(&model->norms, vert_count);
    
    memcpy(m_verts, verts, vert_count*sizeof(*verts));
    memcpy(m_uvs, uvs, vert_count*sizeof(*uvs));
    memcpy(m_norms, norms, vert_count*sizeof(*norms));
    
    mesh_t *mesh = PushOnStack(&model->meshes);
    u16 *m_indices = PushArrayOnStack(&mesh->indices, idx_count);
    memcpy(m_indices, indices, idx_count*sizeof(*indices));
    mesh->material_id = 0;
    
    return assets->free_model_id++;
}

internal u64
CreatePlane(asset_manager_t *assets, v2 dim)
{
    f32 half_x = dim.x*0.5f, half_y = dim.y*0.5f;
    
    v3 verts[] = {
        vec3_init(-half_x, half_y, 0.0f),
        vec3_init( half_x, half_y, 0.0f),
        vec3_init(-half_x,-half_y, 0.0f),
        vec3_init( half_x,-half_y, 0.0f),
        vec3_init( half_x, half_y, 0.0f),
        vec3_init(-half_x, half_y, 0.0f),
        vec3_init( half_x,-half_y, 0.0f),
        vec3_init(-half_x,-half_y, 0.0f),
    };
    
    v3 norms[] = {
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
    };
    
    v2 uvs[] = {
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
    };
    
    u16 indices[] = {
        0, 2, 1, 1, 2, 3,
        4, 6, 5, 5, 6, 7,
    };
    
    return CreateModel(assets, verts, uvs, norms, ARRAY_COUNT(verts),
                       indices, ARRAY_COUNT(indices));
}

internal u64
CreateCube(asset_manager_t *assets, v3 dim)
{
    f32 half_x = dim.x*0.5f, half_y = dim.y*0.5f, half_z = dim.z*0.5f;
    
    v3 verts[] = {
        vec3_init(-half_x, half_y, half_z), // FRONT
        vec3_init( half_x, half_y, half_z),
        vec3_init(-half_x,-half_y, half_z),
        vec3_init( half_x,-half_y, half_z),
        vec3_init( half_x, half_y, half_z), // RIGHT
        vec3_init( half_x, half_y,-half_z),
        vec3_init( half_x,-half_y, half_z),
        vec3_init( half_x,-half_y,-half_z),
        vec3_init(-half_x, half_y,-half_z), // LEFT
        vec3_init(-half_x, half_y, half_z),
        vec3_init(-half_x,-half_y,-half_z),
        vec3_init(-half_x,-half_y, half_z),
        vec3_init( half_x, half_y,-half_z), // BACK
        vec3_init(-half_x, half_y,-half_z),
        vec3_init( half_x,-half_y,-half_z),
        vec3_init(-half_x,-half_y,-half_z),
        vec3_init(-half_x, half_y,-half_z), // TOP
        vec3_init( half_x, half_y,-half_z),
        vec3_init(-half_x, half_y, half_z),
        vec3_init( half_x, half_y, half_z),
        vec3_init(-half_x,-half_y, half_z), // BOTTOM
        vec3_init( half_x,-half_y, half_z),
        vec3_init(-half_x,-half_y,-half_z),
        vec3_init( half_x,-half_y,-half_z),
    };
    
    v3 norms[] = {
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(0.0f, 0.0f, 1.0f),
        vec3_init(1.0f, 0.0f, 0.0f),
        vec3_init(1.0f, 0.0f, 0.0f),
        vec3_init(1.0f, 0.0f, 0.0f),
        vec3_init(1.0f, 0.0f, 0.0f),
        vec3_init(-1.0f, 0.0f, 0.0f),
        vec3_init(-1.0f, 0.0f, 0.0f),
        vec3_init(-1.0f, 0.0f, 0.0f),
        vec3_init(-1.0f, 0.0f, 0.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 0.0f, -1.0f),
        vec3_init(0.0f, 1.0f, 0.0f),
        vec3_init(0.0f, 1.0f, 0.0f),
        vec3_init(0.0f, 1.0f, 0.0f),
        vec3_init(0.0f, 1.0f, 0.0f),
        vec3_init(0.0f, -1.0f, 0.0f),
        vec3_init(0.0f, -1.0f, 0.0f),
        vec3_init(0.0f, -1.0f, 0.0f),
        vec3_init(0.0f, -1.0f, 0.0f),
    };
    
    v2 uvs[] = {
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
        vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f),
        vec2_init(0.0f, 1.0f),
        vec2_init(1.0f, 1.0f),
    };
    
    u16 indices[] = {
        0, 2, 1, 1, 2, 3,
        4, 6, 5, 5, 6, 7,
        8, 10, 9, 9, 10, 11,
        12, 14, 13, 13, 14, 15,
        16, 18, 17, 17, 18, 19,
        20, 22, 21, 21, 22, 23,
    };
    
    return CreateModel(assets, verts, uvs, norms, ARRAY_COUNT(verts),
                       indices, ARRAY_COUNT(indices));
}

internal void
PushModel(render_buffer *rb, asset_manager_t *assets, u64 model_id, m4 transform)
{
    model_t *model = assets->models+model_id;
    u32 vert_idx = GetStackCount(rb->vertices);
    
    v3 *verts = PushArrayOnStack(&rb->vertices, GetStackCount(model->verts));
    memcpy(verts, model->verts, GetStackCount(model->verts)*sizeof(*verts));
    v2 *tex_coords = PushArrayOnStack(&rb->uvs, GetStackCount(model->tex_coords));
    memcpy(tex_coords, model->tex_coords, GetStackCount(model->tex_coords)*sizeof(*tex_coords));
    v3 *norms = PushArrayOnStack(&rb->normals, GetStackCount(model->norms));
    memcpy(norms, model->norms, GetStackCount(model->norms)*sizeof(*norms));
    
    mesh_t *mesh = NULL;
    u32 idx_count = 0;
    u16 *indices = NULL;
    for(u32 m_idx = 0; m_idx < GetStackCount(model->meshes); m_idx++)
    {
        mesh = model->meshes+m_idx;
        
        idx_count = GetStackCount(mesh->indices);
        PushRenderCommand(rb, mesh->material_id, transform, idx_count, PRIMITIVE_TRIANGLES);
        
        indices = PushArrayOnStack(&rb->indices, idx_count);
        for(u32 i_idx = 0; i_idx < idx_count; i_idx++)
            indices[i_idx] = vert_idx + mesh->indices[i_idx];
    }
}