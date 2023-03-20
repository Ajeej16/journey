
internal void
InitLoadedCode(loaded_code *code, void **functionTable,
               char **functionNames, u32 functionCount,
               char *buildDir, char *dllName, char *tempName)
{
    code->dll = NULL;
    CstrCatMany(code->dllPath, buildDir, "\\", dllName);
    code->tempDLLName = tempName;
    code->tempDLLNum = 0;
    code->functionCount = functionCount;
    code->functionNames = functionNames;
    code->functions = functionTable;
    code->isValid = 0;
}

internal void
UnloadCode(loaded_code *code)
{
    if (code->dll)
    {
        UnloadLib(code->dll);
        code->dll = 0;
    }
    
    code->isValid = 0;
}

internal void
LoadCode(loaded_code *code, char *buildDir)
{
    char *dllPath = code->dllPath;
    char tempDLLPath[FILENAME_MAX];
    char *tempDLLName = code->tempDLLName;
    char number[4];
    char ext[4];
    
    u32 periodIndex = CstrFindLast(tempDLLName, '.');
    char *name = malloc(periodIndex+1);
    memcpy(name, tempDLLName, periodIndex);
    name[periodIndex] = '\0';
    memcpy(ext, tempDLLName+periodIndex+1, 3);
    ext[3] = '\0';
    
    for(u32 attempt = 0;
        attempt < 128;
        attempt++)
    {
        tempDLLPath[0] = '\0';
        sprintf(number, "%d", code->tempDLLNum);
        
        CstrCatMany(tempDLLPath, buildDir, "\\", 
                    name, number, ".", ext);
        
        if(++code->tempDLLNum >= 1024)
            code->tempDLLNum = 0;
        
        if(CreateCopyFile(dllPath, tempDLLPath))
            break;
    }
    
    free(name);
    
    code->dll = LoadLib(tempDLLPath);
    if(code->dll)
    {
        code->isValid = 1;
        for(u32 funcIdx = 0;
            funcIdx < code->functionCount;
            funcIdx++)
        {
            void *function = LoadFunction(code->dll,
                                          code->functionNames[funcIdx]);
            if(function)
                code->functions[funcIdx] = function;
            else
                code->isValid = 0;
        }
    }
    
    if(!code->isValid)
        UnloadCode(code);
}