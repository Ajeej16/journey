
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
AddShader(platform_read_file *readFile, render_buffer *rb, char *path)
{
    u32 id = rb->shaderCount + rb->shaderEntryCount;
    
    u8 *code = 0;
    readFile(path, &code, NULL);
    
    shader_entry *entry = rb->shaderEntries + rb->shaderEntryCount++;
    entry->code = code;
    
    return id;
}

internal render_cmd *
PushRenderCommand(render_buffer *rb, u32 textureID, 
                  u32 indicesCount, u32 primitiveType)
{
    render_cmd *rc = GetStackLast(rb->cmds);
    if(!rc || rc->primitiveType != primitiveType || rc->textureID != textureID)
    {
        rc = PushOnStack(&rb->cmds);
        rc->indicesIdx = GetStackCount(rb->indices);
        rc->indicesCount = 0;
        rc->textureID = textureID;
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

internal void
PushTexturedQuad(render_buffer *rb, v3 pos, v2 dim, v2 uv[4],
                 u32 textureID, color tint)
{
    v3 quadVerts[4];
    QuadToVerts(quadVerts, pos, dim);
    
    PushRenderCommand(rb, textureID, 6, PRIMITIVE_TRIANGLES);
    
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
}

internal void
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
    
}