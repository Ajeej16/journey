
#ifndef JOY_PLATFORM_H
#define JOY_PLATFORM_H

#define PLATFORM_READ_FILE(name) void name(char *path, void **outData, u64 *outSize)
typedef PLATFORM_READ_FILE(platform_read_file);

typedef struct platform_functions {
    platform_read_file *readFile;
} platform_functions;

#define INIT_APP(name) void name(platform_functions *platFunctions, render_buffer *rb)
typedef INIT_APP(init_app);

#define UPDATE_AND_RENDER(name) void name(render_buffer *rb, input_state *inputState)
typedef UPDATE_AND_RENDER(update_and_render);

typedef struct app_function_table {
    init_app *initApp;
    update_and_render *updateAndRender;
} app_function_table;

global char *appFunctionNames[] = {
    "InitApp",
    "UpdateAndRender"
};

#endif //JOY_PLATFORM_H
