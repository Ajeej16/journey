
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
#include "joy_octree.h"

global float speed = 0.15f;
global float mouse_sen = 0.3f;

global float yaw = -90.0f;
global float pitch = 0.0f;

global u64 bag_id = 0;
global u64 plane_id = 0;
global u64 cube_id = 0;
global u64 shader_id = 0;

global u64 player = 0;
global u64 player1 = 0;

global collision_handler_t collision_handler = {0};
global entity_manager_t *entities = NULL;
global debug_octree_t octree = {0};
global octant_t *octree_root = NULL;

global STACK(u64) *boxes = 0;

INIT_APP(InitApp)
{
    srand(time(NULL));
    
    // TODO(ajeej): replace hard coded widths and heights
    InitCamera(&rb->cam, vec3_init(0.0f, 0.0f, 200.0f), 0.5f, 45.0f, 800, 600);
    shader_id = AddShader(platFunctions, assets, "..\\shaders\\test.glsl");
    
    entities = CreateEntityManager(4);
    
    plane_id = CreatePlane(assets, vec2_init(3.0f, 3.0f));
    cube_id = CreateCube(assets, vec3_init(1.0f, 1.0f, 1.0f));
    u32 small_cube_id = CreateCube(assets, vec3_init(1.0f, 1.0f, 1.0f));
    u32 red = AddMaterial(assets, "red", 0, 0, COLOR(255, 0, 0, 255), COLOR(255, 0, 0, 255));
    UpdateMaterial(assets, small_cube_id, red);
    
    bag_id = AddModel(platFunctions, assets, "..\\models\\backpack\\backpack.obj",
                      shader_id);
    
    player = CreateEntity(entities, vec3_init(-10.0f, 0.0f, 0.0f), 1, cube_id,
                          (box_info_t){ vec3_init(3.0f, 3.0f, 3.0f) }, BOX_RECT);
    player1 = CreateEntity(entities, vec3_init(10.0f, 0.0f, 0.0f), 10, cube_id,
                           (box_info_t){ vec3_init(3.0f, 3.0f, 3.0f) }, BOX_RECT);
    
    UpdateVelocity(entities, player, vec3_init(5.0f, 0.0f, 0.0f));
    
    i32 a = 58.0f;
    i32 neg[] = { 1, -1 };
    u32 rand_idx = 0;
    for(u32 i = 0; i < 1000; i++)
    {
        rand_idx = rand() % 2;
        f32 x = ((float)rand()/(float)(RAND_MAX)) * a * neg[rand_idx];
        rand_idx = rand() % 2;
        f32 y = ((float)rand()/(float)(RAND_MAX)) * a * neg[rand_idx];
        rand_idx = rand() % 2;
        f32 z = ((float)rand()/(float)(RAND_MAX)) * a * neg[rand_idx];
        u64 *id = PushOnStack(&boxes);
        *id = CreateEntity(entities, vec3_init(x, y, z), 1, small_cube_id, 
                           (box_info_t){ 1.0f, 1.0f, 1.0f }, BOX_RECT);
    }
    
    octree_root = ConstructOctree(entities, boxes, GetStackCount(boxes), 120.0f, 2);
    
    octree = InitDebugOctree(assets, cube_id, octree_root, vec3_init(0.0f, 0.0f, 0.0f), 10);
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
    
    if(!TestCollision(entities, player, player1, dt))
    {
        UpdatePhysics(GetPhysics(entities, GetEntity(entities, player)->physics_id), dt);
        UpdatePhysics(GetPhysics(entities, GetEntity(entities, player1)->physics_id), dt);
    }
    
    //PushQuad(rb, vec3_init(-0.5f, -0.5f, 0.0f), vec2_init(0.5f, 0.5f), COLOR(0, 255, 0, 255));
    
    /*PushCube(rb, vec3_init(3.0f, 0.0f, 0.0f), vec3_init(1.0f, 1.0f, 1.0f), COLOR(0, 0, 255, 255));*/
    //PushModel(rb, assets, cube_id);
    
    HandleCollisions(entities, dt);
    
    for(u32 i = 0; i < GetStackCount(boxes); i++)
    {
        DrawEntity(rb, assets, entities, boxes[i]);
    }
    
    
    DrawDebugOctree(rb, assets, octree);
    
    /*DrawEntity(rb, assets, entities, player1);
    DrawEntity(rb, assets, entities, player);*/
}