

internal hash_table_t *
create_hash_table_of_size(u64 size, u32 el_size)
{
    u64 cap = get_next_power_of_two(size)*2;
    u64 table_size = sizeof(hash_table_t)+(sizeof(hash_entry_t)+el_size)*cap;
    hash_table_t *table = malloc(table_size);
    if(!table)
        return NULL;
    
    memset(table, 0, table_size);
    
    table->size = 0;
    table->cap = cap;
    table->entries = (hash_entry_t *)(table+1);
    table->values = (void *)(table->entries+cap);
    table->value_size = el_size;
    
    return table;
}

internal void
free_hash_table(hash_table_t *table)
{
    for(u64 i = 0; i < table->cap; i++) {
        if(table->entries[i].key)
            free(table->entries[i].key);
    }
    
    free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME  1099511628211UL

internal u64
hash_key(char *key)
{
    u64 hash = FNV_OFFSET;
    for(char *p = key; *p; p++) {
        hash ^= (u64)(u8)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

#define GET_VALUE(table, idx) ((u8 *)table->values+idx*table->size)
#define SET_VALUE(table, from, idx) memcpy(GET_VALUE(table, table->value_size), from, table->value_size);\
table->entries[idx].value = GET_VALUE(table, table->value_size);

internal char *
_set_hash_entry(hash_table_t *table, char *key, void *value)
{
    u64 hash = hash_key(key);
    u64 idx = (u64)(hash & (u64)(table->cap-1));
    
    while(table->entries[idx].key != NULL)
    {
        if(strcmp(key, table->entries[idx].key) == 0) {
            SET_VALUE(table, value, idx);
            return table->entries[idx].key;
        }
        
        idx++;
        if(idx >= table->cap)
            idx = 0;
    }
    
    key = strdup(key);
    if(key == NULL)
        return NULL;
    
    table->entries[idx].key = key;
    SET_VALUE(table, value, idx);
    
    table->size++;
    return key;
}

internal u32
expand_hash_table(hash_table_t **table)
{
    hash_table_t *old_table = *table;
    u64 new_cap = old_table->cap*2;
    if(new_cap < old_table->cap)
        return 0;
    
    hash_table_t *new_table = create_hash_table_of_size(new_cap, old_table->value_size);
    if(table == NULL)
        return 0;
    
    for(u64 i = 0; i < old_table->cap; i++)
    {
        hash_entry_t entry = old_table->entries[i];
        if(entry.key != NULL)
            _set_hash_entry(new_table, entry.key, entry.value);
    }
    
    free_hash_table(old_table);
    *table = new_table;
    
    return 1;
}

internal void *
get_hash_entry(hash_table_t *table, char *key)
{
    u64 hash = hash_key(key);
    u64 idx = (u64)(hash & (u64)(table->cap-1));
    
    while(table->entries[idx].key != NULL)
    {
        if(strcmp(key, table->entries[idx].key) == 0)
            return table->entries[idx].value;
        
        idx++;
        if(idx >= table->cap)
            idx = 0;
    }
    
    return NULL;
}

internal char *
set_hash_entry(hash_table_t **table, char *key, void *value)
{
    ASSERT(value != NULL);
    
    if((*table)->size >= (*table)->cap*0.5f) {
        if(!expand_hash_table(table))
            return NULL;
    }
    
    return _set_hash_entry(*table, key, value);
}