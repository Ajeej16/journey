
#ifndef JOY_ENTITY_H
#define JOY_ENTITY_H

typedef enum box_type_t {
    BOX_SPHERE = 0,
    BOX_RECT,
} box_type_t;

typedef union  collision_box_t {
    struct {
        v3 pos;
        v3 radius;
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
} physics_component_t;

typedef struct collision_info_t {
    physics_component_t *obj0;
    physics_component_t *obj1;
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
    physics_component_t physics;
    box_type_t box_type;
    collision_box_t col_box;
} entity_t;


#endif //JOY_ENTITY_H
