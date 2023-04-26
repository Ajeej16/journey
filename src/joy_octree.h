
#ifndef JOY_OCTREE_H
#define JOY_OCTREE_H

typedef struct octant_t {
    aabb_t bounds;
    u32 div;
    u32 max;
    STACK(u64 *)el;
    octant_t *parent;
    octant_t[8] octants;
} octant_t;


internal octant_t *
ConstructOctree(u64 *entity_ids, u32 entity_count, f32 dim, u32 max)
{
    octant_t *root = malloc(sizeof(*root));
    dim *= 0.5f;
    root->bounds = (aabb_t) {
        vec3_init(-dim, -dim, -dim),
        vec3_init(dim,  dim,  dim),
    };
    root->max = max;
    root->parent = NULL;
    root->div = 0;
}

internal void
InsertObject(octant_t *root, u64 id, f32 half_dim)
{
    if(!root->div && GetStackCount(root->el) >= max) {
        root->div = 1;
        v3 p_min = root->bounds.min;
        v3 p_max = root->bounds.max;
        v3 cur_min = vec3_init(p_min.x, p_max.y-half_dim, p_min.z);
        v3 cur_max = vec3_init(p_min.x+half_dim, p_max.y, p_min.x+half_dim);
        
        u32 oct_idx = 0;
        octant_t *cur_oct = NULL;
        for(u32 y = 0; y < 2; y++)
        {
            cur_min.z = p_min.x; cur_max.z = p_min.x+half_dim;
            for(u32 z = 0; z < 2; z++)
            {
                cur_min.x = p_min.x; cur_max.x = p_min.x+half_dim;
                for(u32 x = 0; x < 2; x++, oct_idx++)
                {
                    cur_oct = root->octants+oct_idx;
                    cur_oct->bound = (aabb_t) { cur_min, cur_max };
                    cur_oct->max = root->max;
                    cur_oct->parent = root;
                    cur_oct->div = 0;
                    
                    if(IS_IN_BOUND)
                        InsertObject(cur_oct, id, half_dim*0.5f);
                    
                    cur_min.x += half_dim; cur_max.x += half_dim;
                }
                
                cur_min.z += half_dim; cur_max.z += half_dim;
            }
            cur_min.y += half_dim; cur_max.y += half_dim;
        }
        
        for(u32 el_idx = 0; el_idx < max; el_idx++)
        {
            for(oct_idx = 0; oct_idx < 8; oct_idx++)
            {
                if(IS_IN_BOUND)
                    InsertObject(cur_oct, id, half_dim*0.5f);
            }
        }
    }
    
    InsertObject(root, id, half_dim*0.5f);
}


#endif //JOY_OCTREE_H
