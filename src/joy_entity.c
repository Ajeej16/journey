
internal void
InitEntity(entity_t *entity, v3 pos, u64 model_id)
{
    entity->model_id = model_id;
    entity->physics.pos = pos;
    entity->physics.vel = vec3_init(0.0f, 0.0f, 0.0f);
    entity->physics.force = vec3_init(0.0f, 0.0f, 0.0f);
    entity->physics.mass = 0.1;
    entity->box_type = BOX_RECT;
    entity->col_box = (collision_box_t){
        vec3_init(pos.x-1.5f, pos.y-1.5f, pos.z-1.5f),
        vec3_init(pos.x+1.5f, pos.y+1.5f, pos.z+1.5f),
    };
}

internal void
DrawEntity(render_buffer *rb, asset_manager_t *assets, entity_t *entity)
{
    model_t *model = assets->models+entity->model_id;
    m4 transform = mat4_translate(entity->physics.pos);
    
    PushModel(rb, assets, entity->model_id, transform);
}

internal void
UpdatePhysics(physics_component_t *physics, f64 dt)
{
    v3 r = vec3_scale(physics->force, dt/physics->mass);
    physics->vel = vec3_add(physics->vel, r);
    
    v3 temp = vec3_scale(physics->vel, dt);
    physics->pos = vec3_add(physics->pos, temp);
}

internal u32
TestRectToRect(collision_handler_t *handler, entity_t *ea, entity_t *eb, f64 dt)
{
    v3 norms[6] = {
        vec3_init(-1.0f, 0.0f, 0.0f), vec3_init(1.0f, 0.0f, 0.0f),
        vec3_init( 0.0f,-1.0f, 0.0f), vec3_init(0.0f, 1.0f, 0.0f),
        vec3_init( 0.0f, 0.0f,-1.0f), vec3_init(0.0f, 0.0f, 1.0f)
    };
    f32 dist[6] = {0};
    
    aabb_t abox = ea->col_box;
    aabb_t bbox = eb->col_box;
    physics_component_t aphysics = ea->physics;
    physics_component_t bphysics = eb->physics;
    
    v3 amin = abox.min;
    v3 amax = abox.max;
    v3 bmin = bbox.min;
    v3 bmax = bbox.max;
    
    f64 time_per_step = dt/handler->steps_per_frame;
    
    for(u32 step_idx = 0; 
        step_idx <= handler->steps_per_frame; 
        step_idx++)
    {
        UpdatePhysics(&aphysics, time_per_step);
        UpdatePhysics(&bphysics, time_per_step);
        
        u32 intersect = (!(amin.x > bmax.x || amax.x < bmin.x) &&
                         !(amin.y > bmax.y || amax.y < bmin.y) &&
                         !(amin.z > bmax.z || amax.z < bmin.z));
        
        if(!intersect)
            continue;
        
        if(!handler)
            return 1;
        
        dist[0] = bmax.x - amin.x;
        dist[1] = amax.x - bmin.x;
        dist[2] = bmax.y - amin.y;
        dist[3] = amax.y - bmin.y;
        dist[4] = bmax.z - amin.z;
        dist[5] = amax.z - bmin.z;
        
        f32 min_dist = dist[0];
        v3 norm = norms[0];
        
        for (u32 dir_idx = 1;
             dir_idx < ARRAY_COUNT(dist);
             dir_idx++)
        {
            // TODO(ajeej): what happens when corners collide ?
            if(dist[dir_idx] < min_dist) {
                min_dist = dist[dir_idx];
                norm = norms[dir_idx];
            }
        }
        
        
        collision_info_t *collision = PushOnStack(&handler->collisions);
        collision->obj0 = &ea->physics;
        collision->obj1 = &eb->physics;
        collision->normal = norm;
        collision->depth = min_dist;
        collision->steps = step_idx;
        
        return 1;
    }
    
    return 0;
}

internal i32
CompareCollisionInfo(const void *b0, const void *b1)
{
    collision_info_t *c0 = (collision_info_t *)b0;
    collision_info_t *c1 = (collision_info_t *)b1;
    return c0->steps-c1->steps;
}

internal u32
TestCollision(collision_handler_t *handler, entity_t *ea, entity_t *eb, f64 dt)
{
    if(ea->box_type == BOX_SPHERE && eb->box_type == BOX_SPHERE)
    {
        return 0;//TestSphereToSphere(handler, ea, eb, dt);
    }
    else if(ea->box_type == BOX_SPHERE && eb->box_type == BOX_RECT)
    {
        return 0;//TestSphereToRect(handler, ea, eb, dt);
    }
    else if(ea->box_type == BOX_RECT && eb->box_type == BOX_SPHERE)
    {
        return 0;//TestSphereToRect(handler, eb, ea, dt);
    }
    else
    {
        return TestRectToRect(handler, ea, eb, dt);
    }
}

internal void
HandleCollisions(collision_handler_t *handler, f64 dt)
{
    u32 collision_count = GetStackCount(handler->collisions);
    qsort(handler->collisions, collision_count, sizeof(*handler->collisions), CompareCollisionInfo);
    
    u32 steps_left = 0;
    f64 secs_per_step = dt/handler->steps_per_frame;
    collision_info_t *collision = 0;
    physics_component_t *obj0 = 0;
    physics_component_t *obj1 = 0;
    v3 rel_vel = {0};
    v3 impulse = {0};
    f32 inv_mass_sum = 0;
    for(u32 col_idx = 0;
        col_idx < collision_count;
        col_idx++)
    {
        collision = handler->collisions+col_idx;
        obj0 = collision->obj0;
        obj1 = collision->obj1;
        steps_left = handler->steps_per_frame - collision->steps;
        
        rel_vel = vec3_sub(obj0->vel, obj1->vel);
        inv_mass_sum = 1.0f/obj0->mass + 1.0f/obj1->mass;
        
        f32 impulse_mag = -1.0f*(1.8f) * vec3_dot(rel_vel, collision->normal)/inv_mass_sum;
        impulse = vec3_scale(collision->normal, impulse_mag);
        v3 accel0 = vec3_scale(impulse, 1.0f/obj0->mass);
        v3 accel1 = vec3_scale(impulse, -1.0f/obj1->mass);
        
        obj0->pos = vec3_sub(vec3_add(obj0->pos, 
                                      vec3_scale(obj0->vel, collision->steps*secs_per_step)),
                             vec3_scale(collision->normal, collision->depth));
        obj1->pos = vec3_add(obj1->pos, vec3_scale(obj1->vel, collision->steps*secs_per_step));
        
        obj0->vel = vec3_add(obj0->vel, accel0);
        obj1->vel = vec3_add(obj1->vel, accel1);
        
        obj0->pos = vec3_add(obj0->pos, vec3_scale(obj0->vel, steps_left*secs_per_step));
        obj1->pos = vec3_add(obj1->pos, vec3_scale(obj1->vel, steps_left*secs_per_step));
    }
    
    ClearStack(handler->collisions);
}