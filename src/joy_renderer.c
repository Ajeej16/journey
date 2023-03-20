
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
QuadToVerts(v2 *verts, v2 pos, v2 dim)
{
    
    verts[0] = pos;
    verts[1] = vec2_init(pos.x, pos.y+dim.y);
    verts[2] = vec2_init(pos.x+dim.x, pos.y+dim.y);
    verts[3] = vec2_init(pos.x+dim.x, pos.y);
}

internal void
PushTexturedQuad(render_buffer *rb, v2 pos, v2 dim, v2 uv[4],
                 u32 textureID, color tint)
{
    v2 quadVerts[4];
    QuadToVerts(quadVerts, pos, dim);
    
    PushRenderCommand(rb, textureID, 6, PRIMITIVE_TRIANGLES);
    
    u32 vertIdx = GetStackCount(rb->vertices);
    
    v2 *verts = PushArrayOnStack(&rb->vertices, 4);
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
PushQuad(render_buffer *rb, v2 pos, v2 dim, color tint)
{
    v2 uv[4] = { 
        vec2_init(0.0f, 1.0f), vec2_init(0.0f, 0.0f),
        vec2_init(1.0f, 0.0f), vec2_init(1.0f, 1.0f) 
    };
    
    // TODO(ajeej): pass white texture id
    PushTexturedQuad(rb, pos, dim, uv, 0, tint);
}