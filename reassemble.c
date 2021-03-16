#include <stdio.h>
#include "reassemble.h"

int parse_file(const char* filename, Chunk** dict_chunk) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("ERROR: Unable to open the file\n\t%s\n", filename);
        return -1;
    }

    while (!feof(fp)) {
        parse_packet(fp, dict_chunk);
    }

    fclose(fp);
    printf("Parsed the file successfully\n");
    return 0;
}

int parse_packet(FILE* fp, Chunk** dict_chunk) {
    int seq = 0, len = 0, tmp = 0;

    // get sequence
    for (int i = 0; i < 4; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        seq = seq << 8;
        seq += tmp;
    }

    // get length
    for (int i = 0; i < 2; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        len = len << 8;
        len += tmp;
    }

    // get data
    char* data = malloc(len);
    for (int i = 0; i < len; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        data[i] = tmp;
    }

    add_chunk(dict_chunk, seq, len, data);

    return 0;
}

int reassemble(Chunk** dict_chunk) {
    const char filename[] = "restored_file";
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("ERROR: Unable to open the file\n\t%s\n", filename);
        return -1;
    }

    int seq = 0;
    Chunk* item = NULL;

    while (1) {
        HASH_FIND_INT(*dict_chunk, &seq, item);
        if (item == NULL) break;

        fwrite(item->data, 1, item->len, fp);
        seq++;
    }

    fclose(fp);
    printf("Reassemble successfully\n\toutput file: %s\n", filename);
    return 0;
}

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