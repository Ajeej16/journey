
#ifndef SDL_JOY_RENDERER_H
#define SDL_JOY_RENDERER_H

#define INIT_RENDERER(name) render_buffer *name(SDL_Window *window)
typedef INIT_RENDERER(init_renderer);

#define START_FRAME(name) void name(render_buffer *rb)
typedef START_FRAME(start_frame);

#define END_FRAME(name) void name(SDL_Window *window, render_buffer *rb)
typedef END_FRAME(end_frame);

#define FREE_RENDERER(name) void name(SDL_Window *window)
typedef FREE_RENDERER(free_renderer);

typedef struct render_function_table {
    init_renderer *initRenderer;
    start_frame *startFrame;
    end_frame *endFrame;
    free_renderer *freeRenderer
} render_function_table;

global char *renderFunctionNames[] = {
    "InitRenderer",
    "StartFrame",
    "EndFrame",
    "FreeRenderer",
};

#endif //SDL_JOY_RENDERER_H
