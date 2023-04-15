
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
    
    gl->glGenBuffers(1, &gl->vboID[VBO_COLOR]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_COLOR]);
    
    gl->glGenBuffers(1, &gl->vboID[VBO_INDEX]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_INDEX]);
    
    gl->glBindVertexArray(0);
    
    gl->rb.vertices = NULL;
    gl->rb.uvs = NULL;
    gl->rb.colors = NULL;
    gl->rb.indices = NULL;
    gl->rb.cmds = NULL;
    
    gl->rb.shaderEntryCount = 0;
    gl->rb.shaderCount = 0;
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
        outShader->locs[SHADER_LOC_MATRIX_VIEW] =
            GetUniformLocation(gl, outShader->id, SHADER_MATRIX_VIEW_NAME);
        outShader->locs[SHADER_LOC_MATRIX_PROJECTION] =
            GetUniformLocation(gl, outShader->id, SHADER_MATRIX_PROJECTION_NAME);
    }
}

internal void
SubmitRenderBuffer(gl_renderer *gl)
{
    render_buffer *rb = (render_buffer *)gl;
    
    gl->glBindVertexArray(gl->vaoID);
    
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_POSITION]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->vertices),
                     (f32 *)rb->vertices, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);
    gl->glEnableVertexAttribArray(0);
    
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_UV]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->uvs),
                     (f32 *)rb->uvs, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    gl->glEnableVertexAttribArray(1);
    
    gl->glBindBuffer(GL_ARRAY_BUFFER, gl->vboID[VBO_COLOR]);
    gl->glBufferData(GL_ARRAY_BUFFER, GetStackSize(rb->colors),
                     (u8 *)rb->colors, GL_DYNAMIC_DRAW);
    gl->glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    gl->glEnableVertexAttribArray(2);
    
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->vboID[VBO_INDEX]);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetStackSize(rb->indices),
                     rb->indices, GL_DYNAMIC_DRAW);
    
    u64 max = GetStackCount(rb->cmds);
    render_cmd *cmd = rb->cmds;
    for(u64 i = 0; i < max; i++, cmd++)
    {
        glDrawElements(cmd->primitiveType, cmd->indicesCount,
                       GL_UNSIGNED_SHORT,
                       (void *)(cmd->indicesIdx*sizeof(u16)));
    }
    
    gl->glBindVertexArray(0);
}