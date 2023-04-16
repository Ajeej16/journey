
#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_platform.h"
#include "joy_renderer.c"

global float speed = 0.05f;
global float mouse_sen = 0.5f;

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
    camera *cam = &rb->cam;
    v2 offset = inputState->mouseOffset;
    
    if(GetKeyState_(inputState, KEY_ID_Q, INPUT_DOWN))
    {
        yaw -= mouse_sen;
    }
    if(GetKeyState_(inputState, KEY_ID_E, INPUT_DOWN))
    {
        yaw += mouse_sen;
    }
    
    if(GetKeyState_(inputState, KEY_ID_R, INPUT_DOWN))
    {
        pitch += mouse_sen;
    }
    if(GetKeyState_(inputState, KEY_ID_F, INPUT_DOWN))
    {
        pitch -= mouse_sen;
    }
    
    //yaw += offset.x * mouse_sen;
    //pitch += -offset.y * mouse_sen;
    pitch = MAX(-89.0f, MIN(89.0f, pitch));
    v3 dir = {0};
    dir.x = cos(RADIAN(yaw))*cos(RADIAN(pitch));
    dir.y = sin(RADIAN(pitch));
    dir.z = sin(RADIAN(yaw))*cos(RADIAN(pitch));
    cam->front = dir;
    
    v3 side = vec3_cross(cam->front, cam->up);
    
    if(GetKeyState_(inputState, KEY_ID_W, INPUT_DOWN))
    {
        cam->pos = vec3_add(cam->pos, vec3_scale(rb->cam.front, speed));
    }
    
    if(GetKeyState_(inputState, KEY_ID_S, INPUT_DOWN))
    {
        cam->pos = vec3_sub(cam->pos, vec3_scale(rb->cam.front, speed));
    }
    
    if(GetKeyState_(inputState, KEY_ID_A, INPUT_DOWN))
    {
        cam->pos = vec3_sub(cam->pos, vec3_scale(side, speed));
    }
    
    if(GetKeyState_(inputState, KEY_ID_D, INPUT_DOWN))
    {
        cam->pos = vec3_add(cam->pos, vec3_scale(side, speed));
    }
    
    if(GetKeyState_(inputState, KEY_ID_SPACE, INPUT_DOWN))
    {
        cam->pos = vec3_add(cam->pos, vec3_scale(rb->cam.up, speed));
    }
    
    if(GetKeyState_(inputState, KEY_ID_CTRL, INPUT_DOWN))
    {
        cam->pos = vec3_sub(cam->pos, vec3_scale(rb->cam.up, speed));
    }
    
    UpdateCamera(&rb->cam);
    
    PushQuad(rb, vec3_init(-0.5f, -0.5f, 0.0f), vec2_init(0.5f, 0.5f), COLOR(0, 255, 0, 255));
    
    PushCube(rb, vec3_init(0.0f, 0.0f, 0.0f), vec3_init(1.0f, 1.0f, 1.0f), COLOR(0, 0, 255, 255));
}