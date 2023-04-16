
#include "windows.h"
#include "windowsx.h"

#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "win32_joy_renderer.h"

#include "joy_load.h"

global u8 running = 1;

internal
UNLOAD_LIB()
{
    FreeLibrary(handle);
}

internal
LOAD_LIB()
{
    return LoadLibraryA(filename);
}

internal
LOAD_FUNCTION()
{
    return GetProcAddress(handle, funcName);
}

internal u32
CreateCopyFile(char *filename, char *newFilename)
{
    return CopyFile(filename, newFilename, FALSE);
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

#include "joy_load.c"

internal void
Win32ProcessPendingMessages(HWND window, input_state *inputState)
{
    MSG message = {0};
    
    inputState->mouseOffset = vec2_init(0.0f, 0.0f);
    
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
            
            case WM_MOUSEMOVE:
            {
                v2 oldMousePos = inputState->mousePos;
                
                POINT cur = {0};
                cur.x = GET_X_LPARAM(lParam);
                cur.y = GET_Y_LPARAM(lParam);
                
                ClientToScreen(window, &cur);
                
                inputState->mousePos.x = cur.x;
                inputState->mousePos.y = cur.y;
                
                if(oldMousePos.x == FLT_MAX && oldMousePos.y == FLT_MAX)
                    oldMousePos = inputState->mousePos;
                
                inputState->mouseOffset = vec2_sub(inputState->mousePos,
                                                   oldMousePos);
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
    
    LoadCode(&renderCode, buildDir);
    LoadCode(&appCode, buildDir);
    
    ASSERT(renderCode.isValid && appCode.isValid);
    
    platform_functions platFunctions = {0};
    platFunctions.readFile = Win32ReadFile;
    
    RECT rect = {0};
    GetClientRect(window, &rect);
    v2 winPos = vec2_init(rect.left, rect.top);
    v2 winDim = vec2_init(rect.right-rect.left, rect.bottom-rect.top);
    
    GetWindowRect(window, &rect);
    
    SetCursorPos(rect.left+400, rect.bottom-300);
    
    render_buffer *rb = renderFunctions.initRenderer(renderDC, winPos, winDim);
    
    input_state *inputState = InitInputState();
    inputState->mousePos.x = rect.left+400;
    inputState->mousePos.y = rect.bottom-300;
    
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    
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