
#ifndef WIN32_JOY_RENDERER_H
#define WIN32_JOY_RENDERER_H

#define INIT_RENDERER(name) render_buffer *name(HDC dc, v2 winPos, v2 winDim,\
asset_manager_t *assets)
typedef INIT_RENDERER(init_renderer);

#define START_FRAME(name) void name(platform_functions *plat_funcs, render_buffer *rb, asset_manager_t *assets)
typedef START_FRAME(start_frame);

#define END_FRAME(name) void name(render_buffer *rb, asset_manager_t *assets)
typedef END_FRAME(end_frame);

typedef struct render_function_table {
    init_renderer *initRenderer;
    start_frame *startFrame;
    end_frame *endFrame;
} render_function_table;

global char *renderFunctionNames[] = {
    "InitRenderer",
    "StartFrame",
    "EndFrame",
};

#endif //WIN32_JOY_RENDERER_H
