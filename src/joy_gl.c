
internal void
InitGLRenderer(gl_renderer *gl)
{
    gl->vboID = calloc(VBO_COUNT, sizeof(u32));
    
    gl->glGenVertexArrays(1, &gl->vaoID);
    gl->glBindVertexArray(gl->vaoID);
    
    gl->glGenBuffers(1, &gl->vboID[VBO_POSITION]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_POSITION]);
    
    gl->glGenBuffers(1, &gl->vboID[VBO_UV]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_UV]);
    
    gl->glGenBuffers(1, &gl->vboID[VBO_NORMAL]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_NORMAL]);
    
    gl->glGenBuffers(1, &gl->vboID[VBO_COLOR]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_COLOR]);
    
    gl->glGenBuffers(1, &gl->vboID[VBO_INDEX]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_INDEX]);
    
    gl->glBindVertexArray(0);
    
    gl->rb.vertices = NULL;
    gl->rb.uvs = NULL;
    gl->rb.normals = NULL;
    gl->rb.colors = NULL;
    gl->rb.indices = NULL;
    gl->rb.cmds = NULL;
}

internal i32
GetUniformLocation(gl_renderer *gl, u32 shaderID, char *uniformName)
{
    i32 loc = -1;
    loc = gl->glGetUniformLocation(shaderID, uniformName);
    return loc;
}

internal void
SetUniform(gl_renderer *gl, i32 loc, void *data, u32 type, u32 count)
{
    switch(type)
    {
        case UNIFORM_FLOAT: gl->glUniform1fv(loc, count, (f32 *)data); break;
        case UNIFORM_VEC2: gl->glUniform2fv(loc, count, (f32 *)data); break;
        case UNIFORM_VEC3: gl->glUniform3fv(loc, count, (f32 *)data); break;
        case UNIFORM_VEC4: gl->glUniform4fv(loc, count, (f32 *)data); break;
        case UNIFORM_INT: gl->glUniform1iv(loc, count, (i32 *)data); break;
        case UNIFORM_IVEC2: gl->glUniform2iv(loc, count, (i32 *)data); break;
        case UNIFORM_IVEC3: gl->glUniform3iv(loc, count, (i32 *)data); break;
        case UNIFORM_IVEC4: gl->glUniform4iv(loc, count, (i32 *)data); break;
        case UNIFORM_MATRIX: gl->glUniformMatrix4fv(loc, count, 0, (f32 *)data); break;
        case UNIFORM_SAMPLER2D: gl->glUniform1iv(loc, count, (i32 *)data); break;
    }
}

internal u64
CompileShader(gl_renderer *gl, u8 *code, u32 type)
{
    u64 shader = gl->glCreateShader(type);
    gl->glShaderSource(shader, 1, &code, NULL);
    
    GLint success = 0;
    gl->glCompileShader(shader);
    gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if(success == GL_FALSE)
        ASSERT(0);
    
    return shader;
}

internal u64
LoadShaderCode(gl_renderer *gl, u8 *source)
{
    u64 shaderID = 0;
    
    char *iter = source;
    u32 type = 0;
    
    u64 shaders[2];
    u32 shaderIdx = 0;
    
    iter = CstrFindNext(iter, '#');
    while (*iter != '\0')
    {
        if (iter[1] == 'V' && iter[2] == 'S')
        {
            type = GL_VERTEX_SHADER;
            iter += 3;
        }
        else if (iter[1] = 'F' && iter[2] == 'S')
        {
            type = GL_FRAGMENT_SHADER;
            iter += 3;
        }
        
        iter = CstrFindNext(iter, '#');
        
        char *end = CstrFindNext(iter + 1, '#');
        u64 size = (u64)(end-iter);
        char *code = malloc(size+1);
        memset(code, 0, size+1);
        memcpy(code, iter, size);
        code[size] = 0;
        
        shaders[shaderIdx++] = CompileShader(gl, code, type);
        
        free(code);
        
        iter += size;
    }
    
    shaderID = gl->glCreateProgram();
    gl->glAttachShader(shaderID, shaders[0]);
    gl->glAttachShader(shaderID, shaders[1]);
    
    gl->glLinkProgram(shaderID);
    GLint success = 0;
    gl->glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    
    if(success == GL_FALSE)
        ASSERT(0);
    
    gl->glDetachShader(shaderID, shaders[0]);
    gl->glDeleteShader(shaders[0]);
    gl->glDetachShader(shaderID, shaders[1]);
    gl->glDeleteShader(shaders[1]);
    
    return shaderID;
}

internal void
LoadShader(gl_renderer *gl, shader *outShader, u8 *code)
{
    outShader->id = LoadShaderCode(gl, code);
    
    if(outShader->id > 0)
    {
        // TODO(ajeej): put names in array and loop
        
        for(u32 loc_idx = 0; loc_idx < SHADER_LOC_COUNT; loc_idx++)
            outShader->locs[loc_idx] = GetUniformLocation(gl, outShader->id, shader_var_names[loc_idx]);
    }
}

internal void
GetGLTextureFormat(u32 format, u32 *gl_internal_format,
                   u32 *gl_format, u32 *gl_type)
{
    switch(format)
    {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: {
            *gl_internal_format = GL_R8;
            *gl_format = GL_RED;
            *gl_type = GL_UNSIGNED_BYTE;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: {
            *gl_internal_format = GL_RG8;
            *gl_format = GL_RG;
            *gl_type = GL_UNSIGNED_BYTE;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R5G6B5: {
            *gl_internal_format = GL_RGB565;
            *gl_format = GL_RGB;
            *gl_type = GL_UNSIGNED_SHORT_5_6_5;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8: {
            *gl_internal_format = GL_RGB8;
            *gl_format = GL_RGB;
            *gl_type = GL_UNSIGNED_BYTE;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: {
            *gl_internal_format = GL_RGB5_A1;
            *gl_format = GL_RGBA;
            *gl_type = GL_UNSIGNED_SHORT_5_5_5_1;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: {
            *gl_internal_format = GL_RGBA4;
            *gl_format = GL_RGBA;
            *gl_type = GL_UNSIGNED_SHORT_4_4_4_4;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: {
            *gl_internal_format = GL_RGBA8;
            *gl_format = GL_RGBA;
            *gl_type = GL_UNSIGNED_BYTE;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R32: {
            *gl_internal_format = GL_R32F;
            *gl_format = GL_RED;
            *gl_type = GL_FLOAT;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32: {
            *gl_internal_format = GL_RGB32F;
            *gl_format = GL_RGB;
            *gl_type = GL_FLOAT;
        } break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: {
            *gl_internal_format = GL_RGBA32F;
            *gl_format = GL_RGBA;
            *gl_type = GL_FLOAT;
        } break;
    }
}

internal texture_t
LoadTextureFromImage(gl_renderer *gl, image_t image)
{
    texture_t texture = image;
    u64 id = 0;
    
    if(texture.width != 0 && texture.height != 0)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        glGenTextures(1, (GLuint *)&id);
        glBindTexture(GL_TEXTURE_2D, id);
        
        u32 mip_width = texture.width;
        u32 mip_height = texture.height;
        u32 mip_offset = 0;
        
        for(u32 i = 0; i < texture.mipmaps; i++)
        {
            u32 mip_size = GetPixelDataSize(mip_width, mip_height, texture.format);
            u32 gl_internal_format, gl_format, gl_type;
            GetGLTextureFormat(texture.format, &gl_internal_format,
                               &gl_format, &gl_type);
            
            if(texture.format < PIXELFORMAT_COMPRESSED_DXT1_RGB) {
                glTexImage2D(GL_TEXTURE_2D, i, gl_internal_format,
                             mip_width, mip_height, 0, gl_format,
                             gl_type, (u8 *)texture.data + mip_offset);
            }
            
            if(texture.format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE) {
                GLint swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA,
                                 swizzle_mask);
            }
            else if(texture.format == PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA)
            {
                GLint swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, 
                                 swizzle_mask);
            }
            
            mip_width *= 0.5f;
            mip_height *= 0.5f;
            mip_offset += mip_size;
            
            if(mip_width < 1) mip_width = 1;
            if(mip_height < 1) mip_height = 1;
        }
        
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        if(texture.mipmaps > 1)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
        ASSERT(0);
    
    
    texture.id = id;
    
    return texture;
}

internal texture_t
LoadTexure(gl_renderer *gl, platform_read_file *read_file, char *filename)
{
    texture_t texture = {0};
    
    image_t image = LoadImageFromFile(read_file, filename);
    
    if(image.data != NULL) {
        texture = LoadTextureFromImage(gl, image);
        UnloadImage(image);
    }
    
    return texture;
}

internal texture_t
UnloadTexture(texture_t texture)
{
    glDeleteTextures(1, (GLuint *)&texture.id);
}

internal void
SubmitRenderBuffer(gl_renderer *gl, asset_manager_t *assets)
{
    render_buffer *rb = (render_buffer *)gl;
    
    gl->glBindVertexArray(gl->vaoID);
    
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_POSITION]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->vertices),
                     (f32 *)rb->vertices, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    gl->glEnableVertexAttribArray(0);
    
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_UV]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->uvs),
                     (f32 *)rb->uvs, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    gl->glEnableVertexAttribArray(1);
    
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_NORMAL]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->normals),
                     (u8 *)rb->normals, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
    gl->glEnableVertexAttribArray(2);
    
    /*gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_COLOR]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->colors),
                     (u8 *)rb->colors, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    gl->glEnableVertexAttribArray(2);*/
    
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->vboID[VBO_INDEX]);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetStackSize(rb->indices),
                     rb->indices, GL_DYNAMIC_DRAW);
    
    u64 max = GetStackCount(rb->cmds);
    render_cmd *cmd = rb->cmds;
    
    for(u64 i = 0; i < max; i++, cmd++)
    {
        material_t *material = assets->materials+cmd->materialID;
        
        SetUniform(gl, assets->shaders[0].locs[SHADER_LOC_MATRIX_MODEL],
                   (void *)cmd->transform.elements,
                   UNIFORM_MATRIX, 1);
        
        for(u32 i = 0; i < 2; i++) {
            gl->glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D,
                          assets->tex_ids[material->maps[i].tex_id]);
            
            SetUniform(gl, assets->shaders[0].locs[SHADER_LOC_SAMPLER2D_DIFFUSE+i],
                       (void *)&i, UNIFORM_SAMPLER2D, 1);
        }
        
        glDrawElements(cmd->primitiveType, cmd->indicesCount,
                       GL_UNSIGNED_SHORT,
                       (void *)(cmd->indicesIdx*sizeof(u16)));
        
        for(u32 i = 0; i < 2; i++) {
            gl->glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    gl->glBindVertexArray(0);
}