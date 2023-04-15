
#ifndef SDL_JOY_RENDERER_H
#define SDL_JOY_RENDERER_H

#define INIT_RENDERER(name) render_buffer *name(GLFWwindow *window)
typedef INIT_RENDERER(init_renderer);

#define START_FRAME(name) void name(GLFWwindow *window, render_buffer *rb)
typedef START_FRAME(start_frame);

#define END_FRAME(name) void name(GLFWwindow *window, render_buffer *rb)
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

#endif //SDL_JOY_RENDERER_H
