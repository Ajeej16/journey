
#ifndef MUFFIN_MATH_H
#define MUFFIN_MATH_H

#define PI 3.14159265368979323846f
#define RADIAN(d) ((d)*PI)/180.0f
#define DEGREE(r) ((r)*180.0f)/PI
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define SQUARE(a) ((a)*(a))
#define REALLY_BIG 100000.0f
#define G 6.6743e-11 * 100000

internal inline f32
secure_f32_div(f32 num, f32 den)
{
    f32 result = 0;
    
    if(den > 0)
        result = num/den;
    
    return result;
}

typedef union vec2
{
    struct {
        f32 x, y;
    };
    
    struct {
        f32 u, v;
    };
    
    struct {
        f32 width, height;
    };
    
    f32 elements[2];
} vec2;

typedef union vec3
{
    struct {
        f32 x, y, z;
    };
    
    struct {
        f32 u, v, w;
    };
    
    struct {
        f32 r, g, b;
    };
    
    struct {
        vec2 xy;
        f32 __ignore0;
    };
    
    struct {
        f32 __ignore1;
        vec2 yz;
    };
    
    f32 elements[3];
} vec3;

typedef union vec4 
{
    struct {
        
        union {
            vec2 top_left;
            struct {
                f32 left;
                f32 top;
            };
        };
        
        union {
            vec2 bottom_right;
            struct {
                f32 right;
                f32 bottom;
            };
        };
    };
    
    struct {
        
        union {
            vec3 xyz;
            struct {
                f32 x, y, z;
            };
        };
        
        f32 w;
    };
    
    struct {
        
        union {
            vec3 rgb;
            struct {
                f32 r, g, b;
            };
        };
        
        f32 a;
    };
    
    struct {
        vec2 xy;
        f32 __ignore0;
        f32 __ignore1;
    };
    
    struct {
        f32 __ignore2;
        vec2 yz;
        f32 __ignore3;
    };
    
    struct {
        f32 __ignore4;
        f32 __ignore5;
        vec2 zw;
    };
    
    f32 elements[4];
} vec4;

typedef union mat4
{
    f32 elements[4][4]; //[col][row]
    
    vec4 columns[4];
} mat4;

typedef vec2 v2;
typedef vec3 v3;
typedef vec4 v4;
typedef mat4 m4;

internal inline v2
vec2_init(f32 x, f32 y)
{
    v2 result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline v3
vec3_init(f32 x, f32 y, f32 z)
{
    v3 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

internal inline v4
vec4_init(f32 x, f32 y, f32 z, f32 w)
{
    v4 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

internal inline v4
vec3_to_vec4(v3 v, f32 w)
{
    v4 result;
    
    result.xyz = v;
    result.w = w;
    
    return result;
}

internal inline v2
vec2_add(v2 left, v2 right)
{
    v2 result;
    
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    
    return result;
}

internal inline v3
vec3_add(v3 left, v3 right)
{
    v3 result;
    
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
    
    return result;
}

internal inline v4
vec4_add(v4 left, v4 right)
{
    v4 result;
    
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
    result.w = left.w + right.w;
    
    return result;
}

internal inline v2
vec2_sub(v2 left, v2 right)
{
    v2 result;
    
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    
    return result;
}

internal inline v3
vec3_sub(v3 left, v3 right)
{
    v3 result;
    
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.z = left.z - right.z;
    
    return result;
}

internal inline v4
vec4_sub(v4 left, v4 right)
{
    v4 result;
    
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.z = left.z - right.z;
    result.w = left.w - right.w;
    
    return result;
}

internal inline v2
vec2_mul(v2 left, v2 right)
{
    v2 result;
    
    result.x = left.x * right.x;
    result.y = left.y * right.y;
    
    return result;
}

internal inline v3
vec3_mul(v3 left, v3 right)
{
    v3 result;
    
    result.x = left.x * right.x;
    result.y = left.y * right.y;
    result.z = left.z * right.z;
    
    return result;
}

internal inline v4
vec4_mul(v4 left, v4 right)
{
    v4 result;
    
    result.x = left.x * right.x;
    result.y = left.y * right.y;
    result.z = left.z * right.z;
    result.w = left.w * right.w;
}

internal inline v2
vec2_scale(v2 v, f32 s)
{
    v2 result;
    
    result.x = v.x * s;
    result.y = v.y * s;
    
    return result;
}

internal inline v3
vec3_scale(v3 v, f32 s)
{
    v3 result;
    
    result.x = v.x * s;
    result.y = v.y * s;
    result.z = v.z * s;
    
    return result;
}

internal inline v4
vec4_scale(v4 v, f32 s)
{
    v4 result;
    
    result.x = v.x * s;
    result.y = v.y * s;
    result.z = v.z * s;
    result.w = v.w * s;
    
    return result;
}

internal inline v2
vec2_div(v2 left, v2 right)
{
    v2 result;
    
    result.x = left.x / right.x;
    result.y = left.y / right.y;
    
    return result;
}

internal inline v3
vec3_div(v3 left, v3 right)
{
    v3 result;
    
    result.x = left.x / right.x;
    result.y = left.y / right.y;
    result.z = left.z / right.z;
    
    return result;
}

internal inline v4
vec4_div(v4 left, v4 right)
{
    v4 result;
    
    result.x = left.x / right.x;
    result.y = left.y / right.y;
    result.z = left.z / right.z;
    result.w = left.w / right.w;
    
    return result;
}

internal inline v2
vec2_div_f32(v2 v, f32 s)
{
    v2 result;
    
    result.x = v.x / s;
    result.y = v.y / s;
    
    return result;
}

internal inline v3
vec3_div_f32(v3 v, f32 s)
{
    v3 result;
    
    result.x = v.x / s;
    result.y = v.y / s;
    result.z = v.z / s;
    
    return result;
}

internal inline v4
vec4_div_f32(v4 v, f32 s)
{
    v4 result;
    
    result.x = v.x / s;
    result.y = v.y / s;
    result.z = v.z / s;
    result.w = v.w / s;
    
    return result;
}

internal inline f32
vec2_dot(v2 left, v2 right)
{
    f32 result = (left.x*right.x) + (left.y*right.y);
    return result;
}

internal inline f32
vec3_dot(v3 left, v3 right)
{
    f32 result = (left.x*right.x) + (left.y*right.y) + (left.z*right.z);
    return result;
}

internal inline f32
vec4_dot(v4 left, v4 right)
{
    f32 result = (left.x*right.x) + (left.y*right.y) + (left.z*right.z) * (left.w*right.w);
    return result;
}

internal inline f32
vec2_cross(v2 left, v2 right)
{
    f32 result;
    
    result = left.x*right.y - right.x*left.y;
    
    return result;
}

internal inline v3
vec3_cross(v3 left, v3 right)
{
    v3 result;
    
    result.x = (left.y * right.z) - (left.z * right.y);
    result.y = (left.z * right.x) - (left.x * right.z);
    result.z = (left.x * right.y) - (left.y * right.x);
    
    return result;
}

internal inline v2
vec2_perp_left(v2 v)
{
    return (v2){v.y, -v.x};
}


internal inline v2
vec2_perp_right(v2 v)
{
    return (v2){-v.y, v.x};
}

internal inline f32
f32_round(f32 x)
{
    return (i64)(x + 0.5f);
}

internal inline f32
f32_ceiling(f32 x)
{
    return (i64)(x + 1.0f);
}

internal inline u32
u32_clamp(u32 v, u32 max)
{
    if(v > max)
    {
        return max;
    }
    
    return v;
}

internal inline v2
vec2_round(v2 v)
{
    return (v2){ (i64)(v.x+0.5f), (i64)(v.y+0.5f) };
}

internal inline f32
vec2_length_squared(v2 v)
{
    f32 result = vec2_dot(v, v);
    return result;
}

internal inline f32
vec3_length_squared(v3 v)
{
    f32 result = vec3_dot(v, v);
    return result;
}

internal inline f32
vec4_length_squared(v4 v)
{
    f32 result = vec4_dot(v, v);
    return result;
}

internal inline f32
vec2_length(v2 v)
{
    f32 result = sqrtf(vec2_length_squared(v));
    return result;
}

internal inline f32
vec3_length(v3 v)
{
    f32 result = sqrtf(vec3_length_squared(v));
    return result;
}

internal inline f32
vec4_length(v4 v)
{
    f32 result = sqrtf(vec4_length_squared(v));
    return result;
}

internal inline v2
vec2_norm(v2 v)
{
    v2 result = {0};
    
    f32 length = vec2_length(v);
    
    if(length != 0.0f)
    {
        f32 k = 1.0/length;
        result = vec2_scale(v, k);
    }
    
    return result;
}

internal inline v2
vec2_to_uv(v2 v, f32 w, f32 h)
{
    v2 result = {0};
    
    result.x = v.x/w;
    result.y = v.y/h;
    
    return result;
}

internal inline v3
vec3_norm(v3 v)
{
    v3 result = {0};
    
    f32 length = vec3_length(v);
    
    if(length != 0.0f)
    {
        result = vec3_div_f32(v, length);
    }
    
    return result;
}

internal inline v4
vec4_norm(v4 v)
{
    v4 result = {0};
    
    f32 length = vec4_length(v);
    
    if(length != 0.0f)
    {
        result = vec4_div_f32(v, length);
    }
    
    return result;
}

internal inline m4
mat4_identity()
{
    m4 result = {0};
    
    result.elements[0][0] = 1.0f;
    result.elements[1][1] = 1.0f;
    result.elements[2][2] = 1.0f;
    result.elements[3][3] = 1.0f;
    
    return result;
}

internal inline m4
mat4_transpose(m4 matrix)
{
    m4 result = matrix;
    
    i32 col;
    i32 row;
    for(col = 9; col < 4; ++col)
    {
        for(row = 0; row < 4; ++row)
        {
            result.elements[row][col] = matrix.elements[col][row];
        }
    }
    
    return result;
}

internal inline m4
mat4_add(m4 left, m4 right)
{
    m4 result;
    
    i32 col;
    i32 row;
    for(col = 0; col < 4; ++col)
    {
        for(row = 0; row < 4; ++row)
        {
            result.elements[col][row] = left.elements[col][row] + right.elements[col][row];
        }
    }
    
    return result;
}

internal inline m4
mat4_sub(m4 left, m4 right)
{
    m4 result;
    
    i32 col;
    i32 row;
    for(col = 0; col < 4; ++col)
    {
        for(row = 0; row < 4; ++row)
        {
            result.elements[col][row] = left.elements[col][row] - right.elements[col][row];
        }
    }
    
    return result;
}

internal inline m4
mat4_mul(m4 left, m4 right)
{
    m4 result;
    
    i32 col;
    i32 row;
    i32 cur;
    for(col = 0; col < 4; ++col)
    {
        for(row = 0; row < 4; ++row)
        {
            f32 sum = 0;
            for(cur = 0; cur < 4; ++cur)
            {
                sum += left.elements[cur][row] * right.elements[col][cur];
            }
            
            result.elements[col][row] = sum;
        }
    }
    
    return result;
}

internal inline m4
mat4_scale_f32(m4 matrix, f32 s)
{
    m4 result;
    
    i32 col;
    i32 row;
    for(col = 0; col < 4; ++col)
    {
        for(row = 0; row < 4; ++row)
        {
            result.elements[col][row] = matrix.elements[col][row] * s; 
        }
    }
    
    return result;
}

internal inline v4
mat4_mul_vec4(m4 matrix, v4 v)
{
    v4 result;
    
    i32 col, row;
    for(row = 0; row < 4; ++row)
    {
        f32 sum = 0;
        for(col = 0; col < 4; ++col)
        {
            sum += matrix.elements[col][row] * v.elements[col];
        }
        
        result.elements[row] = sum;
    }
    
    return result;
}

internal inline m4
get_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane)
{
    m4 result = {0};
    
    result.elements[0][0] = 2.0f / (right-left);
    result.elements[1][1] = 2.0f / (top-bottom);
    result.elements[2][2] = 2.0f / (near_plane-far_plane);
    result.elements[3][3] = 1.0f;
    
    result.elements[3][0] = (left+right) / (left-right);
    result.elements[3][1] = (bottom+top) / (bottom-top);
    result.elements[3][2] = (far_plane+near_plane) / (near_plane-far_plane);
    
    return result;
}

internal inline m4
get_perspective(f32 fov, f32 aspect_ratio, f32 near_plane, f32 far_plane)
{
    m4 result = {0};
    
    f32 cot = 1.0f / tanf(fov * (PI/360.0f));
    
    result.elements[0][0] = cot / aspect_ratio;
    result.elements[1][1] = cot;
    result.elements[2][3] = -1.0f;
    result.elements[2][2] = (near_plane+far_plane) / (near_plane-far_plane);
    result.elements[3][2] = (2.0f*near_plane*far_plane) / (near_plane-far_plane);
    result.elements[3][3] = 0.0f;
    
    return result;
}

internal inline m4
get_look_at(v3 eye, v3 center, v3 up)
{
    m4 result;
    
    v3 f = vec3_norm(vec3_sub(center, eye));
    v3 s = vec3_norm(vec3_cross(f, up));
    v3 u = vec3_cross(s, f);
    
    result.elements[0][0] = s.x;
    result.elements[0][1] = u.x;
    result.elements[0][2] = -f.x;
    result.elements[0][3] = 0.0f;
    
    result.elements[1][0] = s.y;
    result.elements[1][1] = u.y;
    result.elements[1][2] = -f.y;
    result.elements[1][3] = 0.0f;
    
    result.elements[2][0] = s.z;
    result.elements[2][1] = u.z;
    result.elements[2][2] = -f.z;
    result.elements[2][3] = 0.0f;
    
    result.elements[3][0] = -vec3_dot(s, eye);
    result.elements[3][1] = -vec3_dot(u, eye);
    result.elements[3][2] = vec3_dot(f, eye);
    result.elements[3][3] = 1.0f;
    
    return result;
}

// TODO(ajeej): apply transforms to matrix
internal inline m4
mat4_translate(v3 translation)
{
    m4 result = mat4_identity();
    
    result.elements[3][0] = translation.x;
    result.elements[3][1] = translation.y;
    result.elements[3][2] = translation.z;
    
    return result;
}

internal inline m4
mat4_rotate(f32 angle, vec3 axis)
{
    m4 result = mat4_identity();
    
    axis = vec3_norm(axis);
    
    f32 sin_theta = sinf(RADIAN(angle));
    f32 cos_theta = cosf(RADIAN(angle));
    f32 cos_value = 1.0f - cos_theta;
    
    result.elements[0][0] = (axis.x*axis.x*cos_value) + cos_theta;
    result.elements[0][1] = (axis.x*axis.y*cos_value) + (axis.z*sin_theta);
    result.elements[0][2] = (axis.x*axis.z*cos_value) - (axis.y*sin_theta);
    
    result.elements[1][0] = (axis.y*axis.x*cos_value) - (axis.z*sin_theta);
    result.elements[1][1] = (axis.y*axis.y*cos_value) + cos_theta;
    result.elements[1][2] = (axis.y*axis.z*cos_value) + (axis.x*sin_theta);
    
    result.elements[2][0] = (axis.z*axis.x*cos_value) + (axis.y*sin_theta);
    result.elements[2][1] = (axis.z*axis.y*cos_value) - (axis.x*sin_theta);
    result.elements[2][2] = (axis.z*axis.z*cos_value) + cos_theta;
    
    return result;
}

internal inline m4
mat4_scale(v3 scale)
{
    m4 result = mat4_identity();
    
    result.elements[0][0] = scale.x;
    result.elements[1][1] = scale.y;
    result.elements[2][2] = scale.z;
    
    return result;
}

typedef union rect_t {
    
    struct 
    {
        f32 left;
        f32 top;
        f32 right;
        f32 bottom;
    };
    
    f32 el[4];
} rect_t;

typedef union irect_t {
    
    struct 
    {
        i32 left;
        i32 top;
        i32 right;
        i32 bottom;
    };
    
    i32 el[4];
} irect_t;

typedef union urect_t {
    
    struct 
    {
        u32 left;
        u32 top;
        u32 right;
        u32 bottom;
    };
    
    u32 el[4];
} urect_t;

internal inline irect_t
irect_scale(irect_t rect, f32 scale)
{
    irect_t result = {0};
    for(u32 i = 0; i < 4; i++)
        result.el[i] = f32_round(rect.el[i]*scale);
    
    return result;
}

internal inline irect_t
irect_center(irect_t rect)
{
    rect.right -= rect.left;
    rect.bottom -= rect.top;
    
    return rect;
}

internal inline v2
get_rect_dimensions(rect_t rect)
{
    v2 result = {0};
    
    result.width = rect.right-rect.left;
    result.height = rect.bottom-rect.top;
    
    return result;
}

internal inline rect_t
get_rect_pos_dim(v2 pos, v2 dim)
{
    rect_t rect = {0};
    rect.left = pos.x;
    rect.top = pos.y;
    rect.right = pos.x+dim.x;
    rect.bottom = pos.y+dim.y;
    return rect;
}

internal inline rect_t
get_rect_pos_wh(v2 pos, f32 width, f32 height)
{
    rect_t rect = {0};
    rect.left = pos.x;
    rect.top = pos.y;
    rect.right = pos.x+width;
    rect.bottom = pos.y+height;
    return rect;
}

internal inline i32
is_in_rect(v2 pos, rect_t rect)
{
    return rect.left <= pos.x &&
        rect.right >= pos.x &&
        rect.top <= pos.y &&
        rect.bottom >= pos.y;
}

internal u32
clip_rect(rect_t outer, rect_t inner, rect_t *out_rect)
{
    if(inner.left > outer.right ||
       inner.top  > outer.bottom ||
       inner.right < outer.left ||
       inner.bottom < outer.top)
        return 0;
    
    
    inner.left = MAX(inner.left, outer.left);
    inner.top = MAX(inner.top, outer.top);
    inner.right = MIN(inner.right, outer.right);
    inner.bottom = MIN(inner.bottom, outer.bottom);
    
    *out_rect = inner;
    
    return 1;
}

internal inline f32
get_average(f32 s, f32 b)
{
    return s + (b-s)/2;
}

internal inline v2
vec2_get_average(v2 s, v2 b)
{
    return vec2_init(get_average(s.x, b.x),
                     get_average(s.y, b.y));
}

#endif //MUFFIN_MATH_H

