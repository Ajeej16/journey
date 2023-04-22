
#ifndef JOY_HASH_H
#define JOY_HASH_H


typedef struct hash_entry_t {
    char *key;
    void *value;
} hash_entry_t;

typedef struct hash_table_t {
    hash_entry_t *entries;
    u64 cap;
    u64 size;
    void *values;
    u32 value_size;
} hash_table_t;

inline internal u32
get_next_power_of_two(u32 n)
{
    u32 p = 1;
    if(n && !(n & (n-1)))
        return n;
    
    while(p < n)
        p <<=1;
    
    return p;
}


#endif //JOY_HASH_H
