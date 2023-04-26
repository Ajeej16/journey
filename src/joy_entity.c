
internal entity_manager_t *
CreateEntityManager(u32 steps_per_frame)
{
    entity_manager_t *result = malloc(sizeof(*result));
    result->entities = NULL;
    result->physics_comps = NULL;
    
    collision_handler_t *col_handler = malloc(sizeof(*col_handler));
    col_handler->steps_per_frame = steps_per_frame;
    col_handler->collisions = NULL;
    result->col_handler = col_handler;
    
    return result;
}

inline internal collision_box_t
InitAABB(v3 pos, v3 dim)
{
    aabb_t result = {0};
    
    v3 half_dim = vec3_scale(dim, 0.5f);
    result.min = vec3_init(pos.x-half_dim.x, pos.y-half_dim.y, pos.z-half_dim.z);
    result.max = vec3_init(pos.x+half_dim.x, pos.y+half_dim.y, pos.z+half_dim.z);
    return result;
}

internal collision_box_t
UpdateAABB(aabb_t aabb, v3 pos)
{
    aabb_t result = {0};
    
    v3 min = aabb.min;
    v3 max = aabb.max;
    v3 half_dim = vec3_init((max.x-min.x)*0.5f,
                            (max.y-min.y)*0.5f,
                            (max.z-min.z)*0.5f);
    
    result.min = vec3_init(pos.x-half_dim.x,
                           pos.y-half_dim.y,
                           pos.z-half_dim.z);
    result.max = vec3_init(pos.x+half_dim.x,
                           pos.y+half_dim.y,
                           pos.z+half_dim.z);
    
    return result;
}

internal void
UpdateCollisionBox(physics_component_t *physics)
{
    if(physics->box_type == BOX_RECT)
        physics->col_box = UpdateAABB(physics->col_box, physics->pos);
    else
        physics->col_box.pos = physics->pos;
}

inline internal collision_box_t
InitSphere(v3 pos, f32 radius)
{
    sphere_t result = {0};
    result.pos = pos;
    result.radius = radius;
    return result;
}

internal u64
CreatePhysicsComponent(entity_manager_t *manager, v3 pos, f32 mass, 
                       box_info_t box_info, box_type_t type)
{
    u64 id = GetStackCount(manager->physics_comps);
    physics_component_t *physics_comp = PushOnStack(&manager->physics_comps);
    
    physics_comp->box_type = type;
    if(type == BOX_SPHERE)
        physics_comp->col_box = InitSphere(pos, box_info.radius);
    else
        physics_comp->col_box = InitAABB(pos, box_info.dim);
    
    physics_comp->mass = mass;
    physics_comp->pos = pos;
    physics_comp->vel = vec3_init(0.0f, 0.0f, 0.0f);
    physics_comp->force = vec3_init(0.0f, 0.0f, 0.0f);
    
    return id;
}

internal u64
CreateEntity(entity_manager_t *manager, v3 pos, f32 mass, u64 model_id,
             box_info_t box_info, box_type_t type)
{
    u64 id = GetStackCount(manager->entities);
    entity_t *entity = PushOnStack(&manager->entities);
    
    entity->model_id = model_id;
    entity->physics_id = CreatePhysicsComponent(manager, pos, mass, box_info, type);
    
    return id;
}

inline entity_t *
GetEntity(entity_manager_t *manager, u64 entity_id)
{
    return manager->entities+entity_id;
}

inline physics_component_t *
GetPhysics(entity_manager_t *manager, u64 physics_id)
{
    return manager->physics_comps+physics_id;
}

inline void
UpdateVelocity(entity_manager_t *manager, u64 entity_id, v3 vel)
{
    v3 *old_vel = &GetPhysics(manager, GetEntity(manager, entity_id)->physics_id)->vel;
    *old_vel = vel;
}

internal void
DrawEntity(render_buffer *rb, 
           asset_manager_t *assets, entity_manager_t *entities,
           u64 entity_id)
{
    entity_t *entity = GetEntity(entities, entity_id);
    model_t *model = GetModel(assets, entity->model_id);
    physics_component_t *physics = GetPhysics(entities, entity->physics_id);
    m4 transform = mat4_translate(physics->pos);
    
    PushModel(rb, assets, entity->model_id, transform);
}

internal void
UpdatePhysics(physics_component_t *physics, f64 dt)
{
    v3 r = vec3_scale(physics->force, dt/physics->mass);
    physics->vel = vec3_add(physics->vel, r);
    
    v3 temp = vec3_scale(physics->vel, dt);
    physics->pos = vec3_add(physics->pos, temp);
    
    UpdateCollisionBox(physics);
}

inline internal u32
TestIntersectionRectToRect(v3 amin, v3 amax,
                           v3 bmin, v3 bmax)
{
    
    return ((amin.x <= bmax.x && amax.x >= bmin.x) &&
            (amin.y <= bmax.y && amax.y >= bmin.y) &&
            (amin.z <= bmax.z && amax.z >= bmin.z));
}

internal u32
TestIntersection(box_type_t atype, collision_box_t cba, 
                 box_type_t btype, collision_box_t cbb)
{
    if(atype == BOX_SPHERE && btype == BOX_SPHERE)
    {
        return 0;//TestSphereToSphere(handler, ea, eb, dt);
    }
    else if(atype == BOX_SPHERE && btype == BOX_RECT)
    {
        return 0;//TestSphereToRect(handler, ea, eb, dt);
    }
    else if(atype == BOX_RECT && btype == BOX_SPHERE)
    {
        return 0;//TestSphereToRect(handler, eb, ea, dt);
    }
    else
    {
        return TestIntersectionRectToRect(cba.min, cba.max,
                                          cbb.min, cbb.max);
    }
}

internal u32
TestRectToRect(entity_manager_t *manager, u64 pa_id, u64 pb_id, f64 dt)
{
    v3 norms[6] = {
        vec3_init(-1.0f, 0.0f, 0.0f), vec3_init(1.0f, 0.0f, 0.0f),
        vec3_init( 0.0f,-1.0f, 0.0f), vec3_init(0.0f, 1.0f, 0.0f),
        vec3_init( 0.0f, 0.0f,-1.0f), vec3_init(0.0f, 0.0f, 1.0f)
    };
    f32 dist[6] = {0};
    
    collision_handler_t *handler = manager->col_handler;
    physics_component_t *pa = GetPhysics(manager, pa_id);
    physics_component_t *pb = GetPhysics(manager, pb_id);
    aabb_t abox = pa->col_box;
    aabb_t bbox = pb->col_box;
    physics_component_t aphysics = *pa;
    physics_component_t bphysics = *pb;
    
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
        
        u32 intersect = TestIntersectionRectToRect(amin, amax,
                                                   bmin, bmax);
        
        if(!intersect)
            continue;
        
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
        collision->obj0_id = pa_id;
        collision->obj1_id = pb_id;
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
TestCollision(entity_manager_t *manager, u64 ea, u64 eb, f64 dt)
{
    u64 aphysics = GetEntity(manager, ea)->physics_id;
    u64 bphysics = GetEntity(manager, eb)->physics_id;
    physics_component_t *pa = GetPhysics(manager, aphysics);
    physics_component_t *pb = GetPhysics(manager, bphysics);
    
    if(pa->box_type == BOX_SPHERE && pb->box_type == BOX_SPHERE)
    {
        return 0;//TestSphereToSphere(handler, ea, eb, dt);
    }
    else if(pa->box_type == BOX_SPHERE && pb->box_type == BOX_RECT)
    {
        return 0;//TestSphereToRect(handler, ea, eb, dt);
    }
    else if(pa->box_type == BOX_RECT && pb->box_type == BOX_SPHERE)
    {
        return 0;//TestSphereToRect(handler, eb, ea, dt);
    }
    else
    {
        return TestRectToRect(manager, aphysics, bphysics, dt);
    }
}

internal void
HandleCollisions(entity_manager_t *manager, f64 dt)
{
    collision_handler_t *handler = manager->col_handler;
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
        obj0 = GetPhysics(manager, collision->obj0_id);
        obj1 = GetPhysics(manager, collision->obj1_id);
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
        
        UpdateCollisionBox(obj0);
        UpdateCollisionBox(obj1);
    }
    
    ClearStack(handler->collisions);
}