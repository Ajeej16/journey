
#ifndef JOY_UTILS_H
#define JOY_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#define internal static
#define global static
#define local static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define ARRAY_COUNT(a) (sizeof((a))/sizeof((a)[0]))
#define ASSERT(x) if(!(x)) { *(i32 *)0 = 0; }

// NOTE(ajeej): includes null character
internal u64
CstrLen(char *str)
{
    u64 len = 1;
    while(*str++) len++;
    return len;
}

internal char *
CstrCat(char *dst, char *src)
{
    while(*dst) dst++;
    while(*dst++ = *src++);
    return --dst;
}

#define CstrCatMany(d, ...)\
__CstrCatMany(d, __VA_ARGS__, (char *)0)

internal void
__CstrCatMany(char *dst, ...)
{
    va_list ap;
    char *str;
    char *iter = dst;
    
    va_start(ap, dst);
    for(str = va_arg(ap, char *);
        str;
        str = va_arg(ap, char *))
        iter = CstrCat(iter, str);
    va_end(ap);
}

internal u64
CstrFindLast(char *src, char c)
{
    u64 result = 0;
    u64 idx = 0;
    while(*src)
    {
        if(*src++ == c)
            result = idx;
        idx++;
    }
    
    return result;
}

internal char *
CstrFindNext(char *src, char c)
{
    while(*src != '\0' && *src != c) src++;
    return src;
}

typedef struct stack_header {
    u32 capacity;
    u32 count;
} stack_header;

#define INITIAL_STACK_CAPACITY 16

#define GetStackHeader(bp) ((stack_header *)((u8 *)bp - sizeof(stack_header)))
#define GetStackCapacity(bp) ((bp) ? GetStackHeader(bp)->capacity : 0)
#define __GetStackSize(bp, es) ((bp) ? (GetStackHeader(bp)->count*es) : 0)
#define GetStackSize(bp) __GetStackSize(bp, sizeof(*bp))
#define GetStackCount(bp) ((bp) ? GetStackHeader(bp)->count : 0)
#define GetStackLast(bp) ((bp && GetStackHeader(bp)->count != 0) ? bp+(GetStackHeader(bp)->count-1) : 0)
#define ClearStack(bp) ((bp) ? (GetStackHeader(bp)->count = 0) : 0)
#define FreeStack(bp) free(GetStackHeader(bp))

internal i32
__ResizeStack(void **bpp, u32 newCap, u32 elSize)
{
    void *bp = *bpp;
    if (newCap == 0) newCap = INITIAL_STACK_CAPACITY;
    u32 cap = GetStackCapacity(bp);
    if (newCap == cap) return 1;
    
    u32 size = __GetStackSize(bp, elSize)+sizeof(stack_header);
    u32 newSize = elSize*newCap+sizeof(stack_header);
    
    u32 uninitialized = 0;
    if (bp == NULL) uninitialized = 1;
    
    stack_header *head = realloc(uninitialized ? NULL : GetStackHeader(bp), newSize);
    if (head == NULL) return 0;
    
    head->capacity = newCap;
    if (uninitialized) { head->count = 0; }
    
    *bpp = (void *)((u8 *)head + sizeof(*head));
    
    return 1;
}

#define fitStack(bpp, c) __FitStack((void **)bpp, c, sizeof(**bpp))
internal void *
__FitStack(void **bpp, u32 count, u32 elSize)
{
    void *bp = *bpp;
    
    if(bp == NULL) goto resize;
    
    stack_header *head = GetStackHeader(bp);
    count += head->count;
    
    if(count > head->capacity)
    {
        resize: {}
        u32 newCap = (bp != NULL && head->capacity) ? head->capacity * 2 : INITIAL_STACK_CAPACITY;
        
        if(count > newCap)
            newCap = count;
        
        if(!__ResizeStack(&bp, newCap, elSize)) return NULL;
    }
    
    *bpp = bp;
    return bp;
}

#define PushOnStack(bpp) __PushOnStack((void **)bpp, 1, sizeof(**bpp))
#define PushArrayOnStack(bpp, c) __PushOnStack((void **)bpp, c, sizeof(**bpp))
internal void *
__PushOnStack(void **bpp, u32 count, u32 elSize)
{
    void *bp = __FitStack(bpp, count, elSize);
    if(bp == NULL) return NULL;
    stack_header *head = GetStackHeader(bp);
    
    void *element = (void *)((u8 *)bp + elSize*head->count);
    memset(element, 0, count*elSize);
    head->count += count;
    return element;
}

#define PopFromStack(bp) GetStackHeader(bp)->count -= 1;
#define PopArrayFromStack(bp, c) GetStackHeader(bp)->count -= c;

#define STACK(s) s

#endif //JOY_UTILS_H
