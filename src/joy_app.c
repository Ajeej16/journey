
#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "joy_renderer.c"

INIT_APP(InitApp)
{
    // TODO(ajeej): replace hard coded widths and heights
    InitCamera(&rb->cam, vec3_init(0.0f, 0.0f, 0.0f), 0.5f, 45.0f, 800, 600);
    AddShader(platFunctions->readFile, rb, "W:\\journey\\shaders\\test.glsl");
}

UPDATE_AND_RENDER(UpdateAndRender)
{
    if(GetKeyState_(inputState, KEY_ID_W, INPUT_DOWN))
    {
        rb->cam.pos.z -= 0.05f;
    }
    
    if(GetKeyState_(inputState, KEY_ID_S, INPUT_DOWN))
    {
        rb->cam.pos.z += 0.05f;
    }
    
    if(GetKeyState_(inputState, KEY_ID_A, INPUT_DOWN))
    {
        rb->cam.pos.x -= 0.05f;
    }
    
    if(GetKeyState_(inputState, KEY_ID_D, INPUT_DOWN))
    {
        rb->cam.pos.x += 0.05f;
    }
    
    if(GetKeyState_(inputState, KEY_ID_SPACE, INPUT_DOWN))
    {
        rb->cam.pos.y += 0.05f;
    }
    
    if(GetKeyState_(inputState, KEY_ID_CTRL, INPUT_DOWN))
    {
        rb->cam.pos.y -= 0.05f;
    }
    
    UpdateCamera(&rb->cam);
    
    PushQuad(rb, vec2_init(-0.5f, -0.5f), vec2_init(1.0f, 1.0f), COLOR(0, 0, 0, 255));
}