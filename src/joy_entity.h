
#ifndef JOY_ENTITY_H
#define JOY_ENTITY_H

typedef union box_info_t {
    v3 dim;
    f32 radius;
} box_info_t;

typedef enum box_type_t {
    BOX_SPHERE = 0,
    BOX_RECT,
} box_type_t;

typedef union  collision_box_t {
    struct {
        v3 pos;
        f32 radius;
    };
    
    struct {
        v3 min;
        v3 max;
    };
    
} collision_box_t;

typedef collision_box_t sphere_t;
typedef collision_box_t aabb_t;

typedef struct physics_component_t {
    f32 mass;
    v3 pos;
    v3 vel;
    v3 force;
    
    box_type_t box_type;
    collision_box_t col_box;
} physics_component_t;

typedef struct collision_info_t {
    u64 obj0_id;
    u64 obj1_id;
    v3 normal;
    f32 depth;
    u32 steps;
} collision_info_t;

typedef struct collision_handler_t {
    STACK(collision_info_t) *collisions;
    u32 steps_per_frame;
} collision_handler_t;

typedef struct entity_t {
    u64 model_id;
    u64 physics_id;
} entity_t;

typedef struct entity_manager_t {
    STACK(entity_t) *entities;
    STACK(physics_component_t) *physics_comps;
    collision_handler_t *col_handler;
} entity_manager_t;


#endif //JOY_ENTITY_H
