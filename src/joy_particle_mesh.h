
#ifndef JOY_PARTICLE_MESH_H
#define JOY_PARTICLE_MESH_H

typedef struct particle_t {
    u64 id;
    v3 pos;
    f32 mass;
} particle_t;

typedef struct old_info_t {
    u64 x, y, z;
    v3 pos;
    f32 mass;
} old_info_t;

typedef struct pm_grid_t {
    u32 dim;
    f32 cell_size;
    v3 grid_disp;
    
    f32 *greens_function;
    f32 *density_grid;
    old_info_t *id_to_grid;
} pm_grid_t;


#define POINT_TO_IDX(x, y, z, dim) ((x) + (y)*dim + (z)*dim*dim)

internal void
AddToDensityField(f32 *density_grid, 
                  u32 x, u32 y, u32 z, u32 dim,
                  v3 pos, f32 mass)
{
    f32 dx, dy, dz, tx, ty, tz;
    
    dx = pos.x - x;
    dy = pos.y - y;
    dz = pos.z - z;
    tx = 1.0f-dx;
    ty = 1.0f-dy;
    tz = 1.0f-dz;
    f32 wx[] = { tx, dx };
    f32 wy[] = { ty, dy };
    f32 wz[] = { tz, dz };
    
    // TODO(ajeej): fix boundary condition
    for(u32 k = 0; k < 2; k++)
    {
        if(z+k > dim-1) continue;
        for(u32 j = 0; j < 2; j++)
        {
            if(y+j > dim-1) continue;
            for(u32 i = 0; i < 2; i++)
            {
                if(x+i > dim-1) continue;
                density_grid[POINT_TO_IDX(x+i, y+j, z+k, dim)] += mass*wx[i]*wy[j]*wz[k];
            }
        }
    }
}

internal void
SubFromDensityField(f32 *density_grid, 
                    u32 x, u32 y, u32 z, u32 dim,
                    v3 pos, f32 mass)
{
    f32 dx, dy, dz, tx, ty, tz;
    
    dx = pos.x - x;
    dy = pos.y - y;
    dz = pos.z - z;
    tx = 1.0f-dx;
    ty = 1.0f-dy;
    tz = 1.0f-dz;
    f32 wx[] = { tx, dx };
    f32 wy[] = { ty, dy };
    f32 wz[] = { tz, dz };
    
    for(u32 k = 0; k < 2; k++)
    {
        if(z+k > dim-1) continue;
        for(u32 j = 0; j < 2; j++)
        {
            if(y+j > dim-1) continue;
            for(u32 i = 0; i < 2; i++)
            {
                if(x+i > dim-1) continue;
                density_grid[POINT_TO_IDX(x+i, y+j, z+k, dim)] -= mass*wx[i]*wy[j]*wz[k];
            }
        }
    }
}

internal void
FillParticleMeshGrid(pm_grid_t *pm, entity_manager_t *entities)
{
    f32 inv_cell_size = 1.0f/pm->cell_size;
    
    physics_component_t *comp = entities->physics_comps;
    u32 comp_count = GetStackCount(comp);
    
    pm->id_to_grid = malloc(sizeof(*pm->id_to_grid)*comp_count);
    
    particle_t **grid = malloc(sizeof(*grid)*pm->dim*pm->dim*pm->dim);
    memset(grid, 0, sizeof(*grid)*pm->dim*pm->dim*pm->dim);
    
    u32 x, y, z;
    particle_t *particle = NULL;
    u32 idx;
    for(idx = 0; idx < comp_count; idx++, comp++)
    {
        v3 centered_pos = vec3_add(pm->grid_disp, comp->pos);
        
        x = centered_pos.x*inv_cell_size;
        y = centered_pos.y*inv_cell_size;
        z = centered_pos.z*inv_cell_size;
        
        particle = PushOnStack(&grid[z*pm->dim*pm->dim+ y*pm->dim + x]);
        
        particle->id = idx;
        particle->pos = centered_pos;
        particle->mass = comp->mass;
    }
    
    u32 p_count;
    particle = NULL;
    idx = 0;
    for(z = 0; z < pm->dim; z++)
    {
        for(y = 0; y < pm->dim; y++)
        {
            for(x = 0; x < pm->dim; x++, idx++)
            {
                particle = grid[idx];
                p_count = GetStackCount(particle);
                for(u32 p_idx = 0; p_idx < p_count; p_idx++, particle++)
                {
                    pm->id_to_grid[particle->id] = (old_info_t){
                        x, y, z,
                        particle->pos, particle->mass 
                    };
                    AddToDensityField(pm->density_grid, x, y, z, pm->dim,
                                      particle->pos, particle->mass);
                }
            }
        }
    }
    
    for(idx = 0; idx < pm->dim*pm->dim*pm->dim; idx++) {
        if(grid[idx])
            FreeStack(grid[idx]);
    }
    free(grid);
}


internal void
UpdateParticleMeshGrid(fftwf_function_table_t *fftwf_funcs,
                       pm_grid_t *pm, entity_manager_t *entities)
{
    physics_component_t *comp = entities->physics_comps;
    u32 comp_count = GetStackCount(comp);
    for(u32 idx = 0; idx < comp_count; idx++, comp++)
    {
        v3 new_pos = vec3_add(pm->grid_disp, comp->pos);
        v3 old_pos = pm->id_to_grid[idx].pos;
        v3 diff = vec3_sub(new_pos, old_pos);
        if(abs(diff.x) < FLT_EPSILON ||
           abs(diff.y) < FLT_EPSILON ||
           abs(diff.z) < FLT_EPSILON)
        {
            u32 z = idx / (pm->dim*pm->dim);
            u32 y = (idx - z*pm->dim*pm->dim) / pm->dim;
            u32 x = idx % pm->dim;
            
            SubFromDensityField(pm->density_grid,
                                x, y, z, pm->dim,
                                old_pos, pm->id_to_grid[idx].mass);
            
            AddToDensityField(pm->density_grid,
                              x, y, z, pm->dim,
                              new_pos, comp->mass);
            
            pm->id_to_grid[idx].x = new_pos.x/pm->cell_size;
            pm->id_to_grid[idx].y = new_pos.y/pm->cell_size;
            pm->id_to_grid[idx].z = new_pos.z/pm->cell_size;
            pm->id_to_grid[idx].pos = new_pos;
        }
    }
    
    //FillParticleMeshGrid(pm, entities);
    
    u32 fft_size = pm->dim*pm->dim*(pm->dim/2+1);
    u32 dim_cubed = pm->dim*pm->dim*pm->dim;
    f32 *potential_grid = malloc(sizeof(*potential_grid)*dim_cubed);
    fftwf_complex *fdensity_grid = fftwf_funcs->fftwfMalloc(sizeof(*fdensity_grid)*fft_size);
    fftwf_complex *fgreens_function = fftwf_funcs->fftwfMalloc(sizeof(*fgreens_function)*fft_size);
    
    
    fftwf_plan dgrid_plan = fftwf_funcs->fftwfPlanDftR2C3d(pm->dim, pm->dim, pm->dim,
                                                           pm->density_grid, fdensity_grid,
                                                           FFTW_ESTIMATE);
    fftwf_plan gfunc_plan = fftwf_funcs->fftwfPlanDftR2C3d(pm->dim, pm->dim, pm->dim,
                                                           pm->greens_function, fgreens_function,
                                                           FFTW_ESTIMATE);
    fftwf_plan inv_plan = fftwf_funcs->fftwfPlanDftC2R3d(pm->dim, pm->dim, pm->dim,
                                                         fgreens_function, potential_grid,
                                                         FFTW_ESTIMATE);
    
    
    fftwf_funcs->fftwfExecute(dgrid_plan);
    fftwf_funcs->fftwfExecute(gfunc_plan);
    
    
    f32 real0, real1, imag0, imag1;
    u32 i;
    for(u32 i = 0; i < fft_size; i++)
    {
        real0 = fdensity_grid[i][0];
        imag0 = fdensity_grid[i][1];
        real1 = fgreens_function[i][0];
        imag1 = fgreens_function[i][1];
        fgreens_function[i][0] = real0*real1 - imag0*imag1;
        fgreens_function[i][1] = real0*real1 + imag0*imag1;
    }
    
    fftwf_funcs->fftwfExecute(inv_plan);
    
    for(i = 0; i < dim_cubed; i++)
        potential_grid[i] /= dim_cubed;
    
    fftwf_funcs->fftwfDestroyPlan(dgrid_plan);
    fftwf_funcs->fftwfDestroyPlan(gfunc_plan);
    fftwf_funcs->fftwfDestroyPlan(inv_plan);
    fftwf_funcs->fftwfFree(fdensity_grid);
    fftwf_funcs->fftwfFree(fgreens_function);
    
    comp = entities->physics_comps;
    f32 fx, fy, fz;
    u32 x, y, z, nx, ny, nz;
    for(u32 idx = 0; idx < comp_count; idx++, comp++)
    {
        x = pm->id_to_grid[idx].x;
        y = pm->id_to_grid[idx].y;
        z = pm->id_to_grid[idx].z;
        nx = (x+1)%64; ny = (y+1)%64;  nz = (z+1)%64; 
        fx = (potential_grid[POINT_TO_IDX(nx,y,z,pm->dim)]-
              potential_grid[POINT_TO_IDX(x,y,z,pm->dim)])/pm->cell_size;
        fy = (potential_grid[POINT_TO_IDX(x,ny,z,pm->dim)]-
              potential_grid[POINT_TO_IDX(x,y,z,pm->dim)])/pm->cell_size;
        fz = (potential_grid[POINT_TO_IDX(x,y,nz,pm->dim)]-
              potential_grid[POINT_TO_IDX(x,y,z,pm->dim)])/pm->cell_size;
        
        comp->force = vec3_init(-fx, -fy, -fz);
    }
    
    free(potential_grid);
}

internal pm_grid_t
InitParticleMeshGrid(u32 dim, f32 cell_size, f32 alpha,
                     v3 sim_center)
{
    pm_grid_t pm = {0};
    pm.greens_function = malloc(sizeof(*pm.greens_function)*
                                dim*dim*dim);
    pm.density_grid = malloc(sizeof(*pm.density_grid)*
                             dim*dim*dim);
    pm.dim = dim;
    pm.cell_size = cell_size;
    
    f32 real_half_dim = dim*cell_size*0.5f;
    v3 desired_pos = vec3_init(real_half_dim, real_half_dim, real_half_dim);
    v3 disp = vec3_sub(desired_pos, sim_center);
    pm.grid_disp = disp;
    
    // NOTE(ajeej): calculating green function
    // its only dependent on the grid dimensions
    // so we are going to save this for the entire sim
    f32 inv_cell_size = 1.0f/cell_size;
    f32 delta_k = 2*PI*inv_cell_size;
    f32 delta_x = cell_size/dim;
    f32 prefactor = 4*PI*G*delta_x*delta_x*delta_x*alpha;
    f32 half_dim = dim*0.5f;
    
    f32 kx, ky, kz;
    f32 mag_k_sqr;
    u32 idx = 0;
    for(u32 z = 0; z < dim; z++)
    {
        for(u32 y = 0; y < dim; y++)
        {
            for(u32 x = 0; x < dim; x++, idx++)
            {
                kx = x < half_dim ? x * delta_k : (x - dim) *delta_k;
                ky = y < half_dim ? y * delta_k : (y - dim) *delta_k;
                kz = z < half_dim ? z * delta_k : (z - dim) *delta_k;
                mag_k_sqr = kx*kx + ky*ky + kz*kz;
                pm.greens_function[idx] = (mag_k_sqr == 0.0f) ? 0.0f : prefactor/mag_k_sqr;
            }
        }
    }
    
    return pm;
}

#endif //JOY_PARTICLE_MESH_H
