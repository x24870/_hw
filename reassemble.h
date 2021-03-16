#include "uthash.h"

typedef struct chunk {
    int seq;
    int len;
    char* data;
    UT_hash_handle hh;
} Chunk;

int add_chunk(Chunk**, int, int, char*);
void delete_all_chunk(Chunk**);