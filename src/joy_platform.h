
#ifndef JOY_PLATFORM_H
#define JOY_PLATFORM_H

typedef struct file_info_t {
    void *handle;
    u64 size;
} file_info_t;

#define PLATFORM_READ_FILE(name) void name(char *path, void **outData, u64 *outSize)
typedef PLATFORM_READ_FILE(platform_read_file);

#define PLATFORM_OPEN_FILE(name) void name(char *path, file_info_t *file)
typedef PLATFORM_OPEN_FILE(platform_open_file);

// TODO(ajeej): change to to be able to start somewhere
//              change name to read file and the other to read entire file
#define PLATFORM_READ_FILE_OF_SIZE(name) void name(file_info_t *file, u64 size, void **outData)
typedef PLATFORM_READ_FILE_OF_SIZE(platform_read_file_of_size);

#define PLATFORM_CLOSE_FILE(name) void name(file_info_t file)
typedef PLATFORM_CLOSE_FILE(platform_close_file);

#define PLATFORM_UNLOAD_CODE(name) void name(loaded_code *code)
typedef PLATFORM_UNLOAD_CODE(platform_unload_code);

#define PLATFORM_LOAD_CODE(name) void name(loaded_code *code, char *buildDir)
typedef PLATFORM_LOAD_CODE(platform_load_code);

typedef struct platform_functions {
    platform_read_file *readFile;
    platform_open_file *openFile;
    platform_read_file_of_size *readFileOfSize;
    platform_close_file *closeFile;
    platform_unload_code *unloadCode;
    platform_load_code *loadCode;
} platform_functions;

#define INIT_APP(name) void name(platform_functions *platFunctions, render_buffer *rb,\
asset_manager_t *assets, char *buildDir)
typedef INIT_APP(init_app);

#define UPDATE_AND_RENDER(name) void name(render_buffer *rb, input_state *inputState,\
asset_manager_t *assets, f64 dt)
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
