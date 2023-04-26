
#include "joy_utils.h"
#include "joy_math.h"
#include "joy_renderer.h"
#include "joy_input.h"
#include "joy_assets.h"
#include "joy_entity.h"
#include "joy_platform.h"
#include "joy_renderer.c"
#include "joy_assets.c"
#include "joy_obj_loader.c"
#include "joy_entity.c"

global float speed = 0.15f;
global float mouse_sen = 0.3f;

global float yaw = -90.0f;
global float pitch = 0.0f;

global u64 bag_id = 0;
global u64 plane_id = 0;
global u64 cube_id = 0;
global u64 shader_id = 0;

global entity_t player = {0};
global entity_t player1 = {0};

global collision_handler_t collision_handler = {0};

INIT_APP(InitApp)
{
    // TODO(ajeej): replace hard coded widths and heights
    InitCamera(&rb->cam, vec3_init(0.0f, 0.0f, 10.0f), 0.5f, 45.0f, 800, 600);
    shader_id = AddShader(platFunctions, assets, "..\\shaders\\test.glsl");
    
    plane_id = CreatePlane(assets, vec2_init(3.0f, 3.0f));
    cube_id = CreateCube(assets, vec3_init(3.0f, 3.0f, 3.0f));
    
    bag_id = AddModel(platFunctions, assets, "..\\models\\backpack\\backpack.obj",
                      shader_id);
    
    collision_handler.collisions = NULL;
    collision_handler.steps_per_frame = 4;
    
    InitEntity(&player, vec3_init(-10.0f, 0.0f, 0.0f), cube_id);
    InitEntity(&player1, vec3_init(10.0f, 0.0f, 0.0f), cube_id);
    player.physics.vel = vec3_init(0.0f, 0.0f, 0.0f);
    player1.physics.vel = vec3_init(0.0f, 0.0f, 0.0f);
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
        //player.pos.z -= speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_S, INPUT_DOWN))
    {
        cam->pos = vec3_sub(cam->pos, vec3_scale(rb->cam.front, speed));
        //player.pos.z += speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_A, INPUT_DOWN))
    {
        cam->pos = vec3_sub(cam->pos, vec3_scale(side, speed));
        //player.pos.x += speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_D, INPUT_DOWN))
    {
        cam->pos = vec3_add(cam->pos, vec3_scale(side, speed));
        //player.pos.x -= speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_SPACE, INPUT_DOWN))
    {
        cam->pos = vec3_add(cam->pos, vec3_scale(rb->cam.up, speed));
        //player.pos.y += speed;
    }
    
    if(GetKeyState_(inputState, KEY_ID_CTRL, INPUT_DOWN))
    {
        cam->pos = vec3_sub(cam->pos, vec3_scale(rb->cam.up, speed));
        //player.pos.y -= speed;
    }
    
    UpdateCamera(&rb->cam);
    
    if(!TestCollision(&collision_handler, &player, &player1, dt))
    {
        UpdatePhysics(&player.physics, dt);
        UpdatePhysics(&player1.physics, dt);
    }
    
    //PushQuad(rb, vec3_init(-0.5f, -0.5f, 0.0f), vec2_init(0.5f, 0.5f), COLOR(0, 255, 0, 255));
    
    /*PushCube(rb, vec3_init(3.0f, 0.0f, 0.0f), vec3_init(1.0f, 1.0f, 1.0f), COLOR(0, 0, 255, 255));*/
    //PushModel(rb, assets, cube_id);
    
    HandleCollisions(&collision_handler, dt);
    
    player.col_box = (collision_box_t){
        vec3_init(player.physics.pos.x-1.5f, player.physics.pos.y-1.5f, player.physics.pos.z-1.5f),
        vec3_init(player.physics.pos.x+1.5f, player.physics.pos.y+1.5f, player.physics.pos.z+1.5f),
    };
    
    player1.col_box = (collision_box_t){
        vec3_init(player1.physics.pos.x-1.5f, player1.physics.pos.y-1.5f, player1.physics.pos.z-1.5f),
        vec3_init(player1.physics.pos.x+1.5f, player1.physics.pos.y+1.5f, player1.physics.pos.z+1.5f),
    };
    
    DrawEntity(rb, assets, &player1);
    DrawEntity(rb, assets, &player);
}