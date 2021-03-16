#ifndef __REASSEMBLE_H_
#define __REASSEMBLE_H_

#include "uthash.h"

typedef struct chunk {
    int seq;
    int len;
    char* data;
    UT_hash_handle hh;
} Chunk;

int parse_file(const char*, Chunk**);
int parse_packet(FILE*, Chunk**);
int reassemble(Chunk**);

int add_chunk(Chunk**, int, int, char*);
void delete_all_chunk(Chunk**);

#endif