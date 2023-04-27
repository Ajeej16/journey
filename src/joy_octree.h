
#ifndef JOY_OCTREE_H
#define JOY_OCTREE_H

typedef struct octant_t octant_t;
struct octant_t {
    aabb_t bounds;
    u32 div;
    u32 max;
    u32 max_dist_ratio;
    
    v3 center_of_mass;
    f32 total_mass;
    
    STACK(u64 *)els;
    octant_t *parent;
    octant_t *octants;
};

typedef struct debug_octree_t {
    u64 model_id;
    u64 *material_ids;
    u32 material_count;
    octant_t *root;
    v3 pos;
} debug_octree_t;

inline internal debug_octree_t
InitDebugOctree(asset_manager_t *assets, u64 model_id, octant_t *root, v3 pos, u32 material_count)
{
    debug_octree_t result = {0};
    result.model_id = model_id;
    result.root = root;
    result.pos = pos;
    result.material_count = material_count;
    result.material_ids = malloc(sizeof(*result.material_ids)*material_count);
    
    char temp[32];
    int rr, rg, rb;
    for(u32 mat_idx = 0; mat_idx < material_count; mat_idx++)
    {
        rr = rand() % 150;
        rg = rand() % 150;
        rb = rand() % 150;
        sprintf(temp, "octreeR%dG%dB%d", rr, rg, rb);
        result.material_ids[mat_idx] = AddMaterial(assets, temp, 0, 0, COLOR(rr, rg, rb, 100), COLOR(rr, rg, rb, 100));
    }
    
    return result;
}

internal void
DrawOctant(render_buffer *rb, asset_manager_t *assets,
           octant_t *root, u64 model_id, u64 *materials, u32 material_count, u32 *mat_idx,
           v3 pos, f32 half_dim)
{
    if(root->div && root->els == NULL)
    {
        v3 min = root->bounds.min;
        v3 max = root->bounds.max;
        f32 half_half_dim = half_dim*0.5f;
        v3 cur_pos = vec3_init(min.x+half_half_dim, max.y-half_half_dim, min.z+half_half_dim);
        
        u32 oct_idx = 0;
        octant_t *cur_oct = NULL;
        for(u32 y = 0; y < 2; y++)
        {
            cur_pos.z = min.z+half_dim*0.5f;
            for(u32 z = 0; z < 2; z++)
            {
                cur_pos.x = min.x+half_dim*0.5f;
                for(u32 x = 0; x < 2; x++, oct_idx++)
                {
                    cur_oct = root->octants+oct_idx;
                    
                    DrawOctant(rb, assets, cur_oct, model_id, materials, material_count,mat_idx,
                               cur_pos, half_half_dim);
                    
                    cur_pos.x += half_dim;
                }
                
                cur_pos.z += half_dim;
            }
            cur_pos.y -= half_dim;
        }
    }
    
    f32 dim = half_dim*2;
    UpdateMaterial(assets, model_id, materials[*mat_idx]);
    *mat_idx = (++(*mat_idx))%material_count;
    m4 trans = mat4_translate(pos);
    m4 scale = mat4_scale(vec3_init(dim, dim, dim));
    PushModel(rb, assets, model_id, mat4_mul(trans, scale));
}

internal void
DrawDebugOctree(render_buffer *rb, asset_manager_t *assets, debug_octree_t octree)
{
    octant_t *root = octree.root;
    v3 min = root->bounds.min;
    v3 max = root->bounds.max;
    f32 half_dim = (max.x-min.x)*0.5f;
    u32 mat_idx = 0;
    
    DrawOctant(rb, assets, root, octree.model_id, 
               octree.material_ids, octree.material_count, &mat_idx,
               octree.pos, half_dim);
}

internal v3
CalculateCOMExternal(entity_manager_t *entities, u64 *els, u32 el_count, f32 total_mass)
{
    v3 result = {0};
    
    physics_component_t *physics = NULL;
    for(u32 el_idx = 0; el_idx < el_count; el_idx++)
    {
        physics = GetPhysics(entities, els[el_idx]);
        result.x += physics->pos.x * physics->mass;
        result.y += physics->pos.y * physics->mass;
        result.z += physics->pos.z * physics->mass;
    }
    
    result = vec3_scale(result, 1.0f/total_mass);
    return result;
}

internal v3
CalculateCOMInternal(octant_t octants[8], f32 total_mass)
{
    v3 result = {0};
    
    octant_t *octant = NULL;
    for(u32 oct_idx = 0; oct_idx < 8; oct_idx++)
    {
        octant = octants+oct_idx;
        result.x += octant->center_of_mass.x*octant->total_mass;
        result.y += octant->center_of_mass.y*octant->total_mass;
        result.z += octant->center_of_mass.z*octant->total_mass;
    }
    
    result = vec3_scale(result, 1.0f/octant->total_mass);
    return result;
}

typedef enum octant_region_t {
    OCT000 = 0,
    OCT100,
    OCT010,
    OCT110,
    OCT001,
    OCT101,
    OCT011,
    OCT111,
} octant_region_t;

internal u32
GetNewOctant(u32 prev_octant, aabb_t aabb, v3 pos)
{
    u32 new_octant = 0;
    
    if(pos.x > aabb.max.x)
        new_octant |= 1;
    if(pos.y < aabb.min.y)
        new_octant |= 2;
    if(pos.z > aabb.max.z)
        new_octant |= 4;
    
    return new_octant;
}

internal v3
ApplyForce(physics_component_t *to, v3 from_com, f32 from_mass)
{
    v3 r = vec3_sub(to->pos, from_com);
    f32 inv_dist_sqr = 1.0f/vec3_dot(r, r);
    
    to->force = vec3_scale(r, G*to->mass*from_mass*inv_dist_sqr);
}

internal v3
SimulateForces(entity_manager_t *entities, octant_t *root, u64 target)
{
    physics_component_t *comp = GetPhysics(entities, target);
    
    // NOTE(ajeej): Check if the node is external
    if(root->octants = NULL)
    {
        physics_component_t *from = NULL;
        for(u32 el_idx = 0; el_idx < GetStackCount(root->els); el_idx++)
        {
            if(target == root->els[el_idx]) continue;
            from = GetPhysics(entities, root->els[el_idx]);
            ApplyForce(comp, from->pos, from->mass);
        }
    }
    
    aabb_t bounds = root->bounds;
    f32 dist = vec3_length(vec3_sub(comp->pos, root->center_of_mass));
    f32 dist_ratio = (bounds.max.x-bounds.min.x)/dist;
    
    if(dist_ratio < root->max_dist_ratio)
        ApplyForce(comp, root->center_of_mass, root->total_mass);
    else
    {
        for(u32 oct_idx = 0; oct_idx < 8; oct_idx++)
        {
            if(root->octants[oct_idx].total_mass == 0) continue;
            SimulateForces(entities, root, target);
        }
    }
}

internal void
SimulateOctree(entity_manager_t *entities, octant_t *root)
{
    u32 el_count = GetStackCount(entities->physics_comps);
    for(u32 el_idx = 0; el_idx < el_count; el_idx++)
        SimulateForces(entities, root, el_idx);
}

internal v3
InsertObject(entity_manager_t *entities, octant_t *root, u64 id, f32 half_dim)
{
    physics_component_t *physics = GetPhysics(entities, id);
    root->total_mass += physics->mass;
    
    
    u32 oct_idx = 0;
    octant_t *cur_oct = NULL;
    if(root->div && root->els == NULL) {
        
        for(oct_idx = 0; oct_idx < 8; oct_idx++)
        {
            cur_oct = root->octants+oct_idx;
            if(TestIntersection(BOX_RECT, cur_oct->bounds, physics->box_type, physics->col_box))
                InsertObject(entities, cur_oct, id, half_dim*0.5f);
        }
        
        root->center_of_mass = CalculateCOMInternal(root->octants, root->total_mass);
        return root->center_of_mass;
    } 
    
    if(GetStackCount(root->els) >= root->max) 
    {
        root->div = 1;
        root->octants = malloc(sizeof(*root->octants)*8);
        v3 p_min = root->bounds.min;
        v3 p_max = root->bounds.max;
        v3 cur_min = vec3_init(p_min.x, p_max.y-half_dim, p_min.z);
        v3 cur_max = vec3_init(p_min.x+half_dim, p_max.y, p_min.z+half_dim);
        
        for(u32 z = 0; z < 2; z++)
        {
            cur_min.y = p_max.y-half_dim; cur_max.y = p_max.y;
            for(u32 y = 0; y < 2; y++)
            {
                cur_min.x = p_min.x; cur_max.x = p_min.x+half_dim;
                for(u32 x = 0; x < 2; x++, oct_idx++)
                {
                    cur_oct = root->octants+oct_idx;
                    cur_oct->bounds = (aabb_t) { .min = cur_min, .max = cur_max };
                    cur_oct->max = root->max;
                    cur_oct->max_dist_ratio = root->max_dist_ratio;
                    cur_oct->parent = root;
                    cur_oct->div = 0;
                    cur_oct->els = NULL;
                    cur_oct->center_of_mass = vec3_init(0.0f, 0.0f, 0.0f);
                    cur_oct->total_mass = 0;
                    
                    if(TestIntersection(BOX_RECT, cur_oct->bounds, physics->box_type, physics->col_box))
                        cur_oct->center_of_mass = InsertObject(entities, cur_oct, id, half_dim*0.5f);
                    
                    cur_min.x += half_dim; cur_max.x += half_dim;
                }
                
                cur_min.y -= half_dim; cur_max.y -= half_dim;
            }
            cur_min.z += half_dim; cur_max.z += half_dim;
        }
        
        for(u32 el_idx = 0; el_idx < root->max; el_idx++)
        {
            for(oct_idx = 0; oct_idx < 8; oct_idx++)
            {
                cur_oct = root->octants+oct_idx;
                physics = GetPhysics(entities, root->els[el_idx]);
                if(TestIntersection(BOX_RECT, cur_oct->bounds, physics->box_type, physics->col_box))
                    InsertObject(entities, cur_oct, id, half_dim*0.5f);
            }
        }
        
        FreeStack(root->els);
        root->els = NULL;
        
        root->center_of_mass = CalculateCOMInternal(root->octants, root->total_mass);
        return root->center_of_mass;
    }
    
    u64 *new_object = PushOnStack(&root->els);
    *new_object = id;
    
    root->center_of_mass = CalculateCOMExternal(entities, root->els, GetStackCount(root->els), root->total_mass);
    
    return root->center_of_mass;
}

internal void
UpdateOctree(entity_manager_t *entities, octant_t *root, u64 **out_escaped)
{
    u64 *escaped = NULL;
    if(root->octants)
    {
        u64 *free_el = NULL;
        u32 free_count = 0;
        physics_component_t *free_comp = NULL;
        for(u32 oct_idx = 0; oct_idx < 8; oct_idx++)
        {
            UpdateOctree(entities, root->octants+oct_idx, &free_el);
            
            free_count = GetStackCount(free_el);
            for(u32 f_idx = 0; f_idx < free_count; f_idx)
            {
                free_comp = GetPhysics(entities, free_el[f_idx]);
                if(!TestIntersection(BOX_RECT, root->bounds,
                                     free_comp->box_type, free_comp->col_box)) {
                    root->total_mass -= free_comp->mass;
                    escaped = PushOnStack(out_escaped);
                    *escaped = free_el[f_idx];
                    continue;
                }
                
                u32 new_octant_id = GetNewOctant(oct_idx, root->octants[oct_idx].bounds,
                                                 free_comp->pos);
                octant_t *new_octant = root->octants+new_octant_id;
                
                new_octant->center_of_mass = InsertObject(entities, new_octant, free_el[f_idx],
                                                          (root->bounds.max.x-root->bounds.min.x)*0.25f);
            }
        }
        
        root->center_of_mass = CalculateCOMInternal(root->octants, root->total_mass);
    }
    
    // NOTE(ajeej): We are in an external node; update the center of mass
    //              and see of any elements have escaped
    u32 el_count = GetStackCount(root->els);
    u64 *temp = malloc(el_count*sizeof(*temp));
    physics_component_t *comp = NULL;
    u32 el_idx, temp_idx;
    for(u32 el_idx = 0, temp_idx = 0; el_idx < el_count; el_idx++)
    {
        comp = GetPhysics(entities, root->els[el_idx]);
        if(TestIntersection(BOX_RECT, root->bounds, comp->box_type, comp->col_box))
            temp[temp_idx++] = root->els[el_idx];
        else {
            root->total_mass -= GetPhysics(entities, root->els[el_idx])->mass;
            escaped = PushOnStack(out_escaped); *escaped = root->els[el_idx];
        }
    }
    
    ClearStack(root->els);
    u64 *els = PushArrayOnStack(&root->els, temp_idx);
    memcpy(els, temp, sizeof(*temp)*temp_idx);
    free(temp);
    
    root->center_of_mass = CalculateCOMExternal(entities, els, temp_idx, root->total_mass);
}

internal octant_t *
ConstructOctree(entity_manager_t *entities, u64 *physics_ids, u32 entity_count, f32 dim, u32 max)
{
    octant_t *root = malloc(sizeof(*root));
    dim *= 0.5f;
    root->bounds = (aabb_t) {
        .min = vec3_init(-dim, -dim, -dim),
        .max = vec3_init(dim,  dim,  dim),
    };
    root->max = max;
    root->parent = NULL;
    root->div = 0;
    root->els = NULL;
    root->center_of_mass = vec3_init(0.0f, 0.0f, 0.0f);
    root->total_mass = 0;
    
    for(u32 el_idx = 0; el_idx < entity_count; el_idx++)
        root->center_of_mass = InsertObject(entities, root, physics_ids[el_idx], dim);
    
    return root;
}


#endif //JOY_OCTREE_H