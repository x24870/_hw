#include "reassemble.h"

int add_chunk(Chunk** dict_chunks, int seq, int len, char* data) {
    Chunk* new_chunk;

    // check if the key is used
    HASH_FIND_INT(*dict_chunks, &seq, new_chunk);
    if (new_chunk != NULL) {
        return -1;
    }

    new_chunk = malloc(sizeof(Chunk));
    new_chunk->seq = seq;
    new_chunk->len = len;
    new_chunk->data = data;
    HASH_ADD_INT(*dict_chunks, seq, new_chunk);
    return 0;
}

void delete_all_chunk(Chunk** dict_chunks) {
    Chunk *cur, *tmp;

    HASH_ITER(hh, *dict_chunks, cur, tmp) {
        HASH_DEL(*dict_chunks, cur);
        free(cur->data);
        cur->data = NULL;
        free(cur);
        cur = NULL;
    }
}