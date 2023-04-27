
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