
#include "windows.h"

#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "win32_joy_renderer.h"

global u8 running = 1;

// TODO(ajeej): seperate for multi platform use
global char buildDir[MAX_PATH];
typedef struct loaded_code {
    void *dll;
    
    char dllPath[MAX_PATH];
    char *tempDLLName;
    u32 tempDLLNum;
    
    u32 functionCount;
    char **functionNames;
    void **functions;
    
    u32 isValid;
} loaded_code;

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
Win32UnloadCode(loaded_code *code)
{
    if (code->dll)
    {
        FreeLibrary(code->dll);
        code->dll = 0;
    }
    
    code->isValid = 0;
}

internal void
Win32LoadCode(loaded_code *code, char *buildDir)
{
    char *dllPath = code->dllPath;
    char tempDLLPath[MAX_PATH];
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
        
        if(CopyFile(dllPath, tempDLLPath, FALSE))
            break;
    }
    
    free(name);
    
    code->dll = LoadLibraryA(tempDLLPath);
    if(code->dll)
    {
        code->isValid = 1;
        for(u32 funcIdx = 0;
            funcIdx < code->functionCount;
            funcIdx++)
        {
            void *function = GetProcAddress(code->dll,
                                            code->functionNames[funcIdx]);
            if(function)
                code->functions[funcIdx] = function;
            else
                code->isValid = 0;
        }
    }
    
    if(!code->isValid)
        Win32UnloadCode(code);
}

internal 
PLATFORM_READ_FILE(Win32ReadFile)
{
    void *data = 0;
    u64 size = 0;
    
    DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = 0;
    SECURITY_ATTRIBUTES securityAttribs = {
        (DWORD)sizeof(securityAttribs),
        0, 0,
    };
    DWORD creationDisposition = OPEN_EXISTING;
    DWORD flagsAndAttribs = 0;
    HANDLE templateFile = 0;
    
    HANDLE file = CreateFileA(path, desiredAccess,
                              shareMode, &securityAttribs,
                              creationDisposition, flagsAndAttribs,
                              templateFile);
    if(file != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER sizeInt;
        if(GetFileSizeEx(file, &sizeInt) && sizeInt.QuadPart > 0)
        {
            size = sizeInt.QuadPart;
            data = malloc(size+1);
            
            u8 *ptr = (u8 *)data;
            u8 *opl = ptr + size;
            
            for(;;)
            {
                u64 unread = (u64)(opl-ptr);
                DWORD toRead = (DWORD)MIN(unread, (u32)-1);
                DWORD didRead = 0;
                if(!ReadFile(file, ptr, toRead, &didRead, 0))
                    break;
                ptr += didRead;
                if(ptr >= opl)
                    break;
            }
            
            ((u8 *)data)[size] = 0;
            *outData = data;
            if(outSize != NULL)
                *outSize = size;
        }
        else
        {
            // TODO(ajeej): error
        }
        
        CloseHandle(file);
    }
    else
    {
        // TODO(ajeej): error
    }
}

internal void
Win32ProcessPendingMessages(HWND window, input_state *inputState)
{
    MSG message = {0};
    
    for(;;)
    {
        BOOL gotMessage = FALSE;
        DWORD skipMessages[] = { 0x738, 0xFFFFFFFF };
        DWORD lastMessage = 0;
        for(u32 skipIdx = 0;
            skipIdx < ARRAY_COUNT(skipMessages);
            skipIdx++)
        {
            DWORD skip = skipMessages[skipIdx];
            gotMessage = PeekMessage(&message, 0, lastMessage, skip-1, PM_REMOVE);
            if(gotMessage)
                break;
            
            lastMessage = skip+1;
        }
        
        if(!gotMessage)
            break;
        
        u64 wParam = message.wParam;
        u64 lParam = message.lParam;
        
        switch(message.message)
        {
            case WM_QUIT:
            {
                running = 0;
            } break;
            
            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                
                u64 keyCode = wParam;
                u8 isDown = ((lParam & (1<<31)) == 0);
                u64 key = 0;
                u8 keyState = 0;
                
                // TODO(ajeej): Implement other keys
                if(keyCode >= 'A' && keyCode <= 'Z')
                    key = KEY_ID_A + (keyCode - 'A');
                else if(keyCode >= '0' && keyCode <= '9')
                    key = KEY_ID_0 + (keyCode - '0');
                else if(keyCode == VK_SPACE)
                    key = KEY_ID_SPACE;
                else if(keyCode == VK_CONTROL)
                    key = KEY_ID_CTRL;
                
                if(isDown)
                {
                    if((inputState->keyStates[key] & INPUT_DOWN) == 0)
                    {
                        keyState |= INPUT_PRESSED;
                    }
                    keyState |= INPUT_DOWN;
                }
                
                inputState->keyStates[key] = keyState;
                
            } break;
            
            default:
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }
}

internal LRESULT CALLBACK
Win32WindowProcess(HWND window, UINT message,
                   WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    
    switch(message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            running = 0;
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            EndPaint(window, &paint);
        } break;
        
        default:
        {
            result = DefWindowProcA(window, message,
                                    wParam, lParam);
        } break;
    }
    
    return result;
}

int WinMain(HINSTANCE instance, HINSTANCE prevInstance,
            PSTR cmdLine, INT cmdShow)
{
    WNDCLASSA windowClass = {0};
    windowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    windowClass.lpfnWndProc = Win32WindowProcess;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.lpszClassName = "JourneyWindowClass";
    
    if (!RegisterClassA(&windowClass))
        return 1;
    
    HWND window = CreateWindowExA(0,
                                  windowClass.lpszClassName,
                                  "Journey",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  800, 600,
                                  0, 0, instance, 0);
    
    if (!window)
        return 1;
    
    HDC renderDC = GetDC(window);
    
    char buildDir[MAX_PATH];
    GetCurrentDirectory((DWORD)MAX_PATH, (LPTSTR)buildDir);
    
    loaded_code renderCode = {0};
    render_function_table renderFunctions = {0};
    InitLoadedCode(&renderCode, (void **)&renderFunctions,
                   renderFunctionNames,
                   ARRAY_COUNT(renderFunctionNames),
                   buildDir,
                   "win32_journey_opengl.dll",
                   "win32_journey_opengl_temp.dll");
    
    loaded_code appCode = {0};
    app_function_table appFunctions = {0};
    InitLoadedCode(&appCode, (void **)&appFunctions,
                   appFunctionNames,
                   ARRAY_COUNT(appFunctionNames),
                   buildDir,
                   "journey_app.dll",
                   "journey_app_temp.dll");
    
    Win32LoadCode(&renderCode, buildDir);
    Win32LoadCode(&appCode, buildDir);
    
    platform_functions platFunctions = {0};
    platFunctions.readFile = Win32ReadFile;
    
    ASSERT(renderCode.isValid && appCode.isValid);
    
    RECT rect = {0};
    GetClientRect(window, &rect);
    v2 winPos = vec2_init(rect.left, rect.top);
    v2 winDim = vec2_init(rect.right-rect.left, rect.bottom-rect.top);
    
    u8 *shaderSource = NULL;
    Win32ReadFile("W:\\journey\\shaders\\test.glsl", &shaderSource, NULL);
    render_buffer *rb = renderFunctions.initRenderer(renderDC, winPos, winDim, shaderSource);
    
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    
    input_state *inputState = InitInputState();
    
    appFunctions.initApp(&platFunctions, rb);
    
    while(running)
    {
        Win32ProcessPendingMessages(window, inputState);
        
        renderFunctions.startFrame(rb);
        
        appFunctions.updateAndRender(rb, inputState);
        
        renderFunctions.endFrame(rb);
    }
    
    return 0;
}