
#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "joy_renderer.c"

global float speed = 0.05f;
global float mouse_sen = 0.1f;

global float yaw = -90.0f;
global float pitch = 0.0f;

INIT_APP(InitApp)
{
    // TODO(ajeej): replace hard coded widths and heights
    InitCamera(&rb->cam, vec3_init(0.0f, 0.0f, 10.0f), 0.5f, 45.0f, 800, 600);
    AddShader(platFunctions->readFile, rb, "W:\\journey\\shaders\\test.glsl");
}

UPDATE_AND_RENDER(UpdateAndRender)
{
    v2 offset = inputState->mouseOffset;
    yaw += offset.x * mouse_sen;
    pitch += -offset.y * mouse_sen;
    pitch = MAX(-89.0f, MIN(89.0f, pitch));
    v3 dir = {0};
    dir.x = cos(RADIAN(yaw))*cos(RADIAN(pitch));
    dir.y = sin(RADIAN(pitch));
    dir.z = sin(RADIAN(yaw))*cos(RADIAN(pitch));
    rb->cam.front = dir;
    
    if(GetKeyState_(inputState, KEY_ID_W, INPUT_DOWN))
    {
        rb->cam.pos.z -= speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_S, INPUT_DOWN))
    {
        rb->cam.pos.z += speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_A, INPUT_DOWN))
    {
        rb->cam.pos.x -= speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_D, INPUT_DOWN))
    {
        rb->cam.pos.x += speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_SPACE, INPUT_DOWN))
    {
        rb->cam.pos.y += speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_CTRL, INPUT_DOWN))
    {
        rb->cam.pos.y -= speed;
    }
    
    UpdateCamera(&rb->cam);
    
    PushQuad(rb, vec2_init(-0.5f, -0.5f), vec2_init(0.5f, 0.5f), COLOR(0, 255, 0, 255));
}