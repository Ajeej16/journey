
#ifndef JOY_LOAD_H
#define JOY_LOAD_H

typedef struct loaded_code {
    void *dll;
    
    char dllPath[FILENAME_MAX];
    char *tempDLLName;
    u32 tempDLLNum;
    
    u32 functionCount;
    char **functionNames;
    void **functions;
    
    u32 isValid;
} loaded_code;

#define UNLOAD_LIB() void UnloadLib(void *handle)

#define LOAD_LIB() void *LoadLib(char *filename)

#define LOAD_FUNCTION() void *LoadFunction(void *handle, char *funcName)

#endif //JOY_LOAD_H
