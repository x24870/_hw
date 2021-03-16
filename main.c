#include <arpa/inet.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>

typedef struct chunk {
    int seq;
    int len;
    char* data;
    UT_hash_handle hh;
} Chunk;

int add_chunk(Chunk**, int, int, char*);
Chunk* find_chunk(Chunk**, int);
void print_chuncks(Chunk**);
void delete_chunk(Chunk**, Chunk*);
void delete_all_chunk(Chunk**);

size_t got_data(char*, size_t, size_t, void*);

int parse_file_to_dict(char*, Chunk**);
int parse_packet(FILE*, Chunk**);

int main() {
    CURL* curl;
    FILE* fp;
    int result;

    fp = fopen("downloaded_file", "wb");
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "test.jigentec.com:49152");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    result = curl_easy_perform(curl);

    if (result == CURLE_OK) {
        printf("Download successful\n");
    } else {
        printf("ERROR: %s\n", curl_easy_strerror(result));
    }

    fclose(fp);
    curl_easy_cleanup(curl);

    //parse file
    Chunk* dict_chunks = NULL;
    parse_file_to_dict("downloaded_file", &dict_chunks);
}

// process file
int parse_file_to_dict(char* filename, Chunk** dict_chunk) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    while (!feof(fp)) {
        parse_packet(fp, dict_chunk);
    }

    fclose(fp);

    //reassemble
    fp = fopen("output", "w");
    if (fp == NULL) return -1;

    int seq = 0;
    Chunk* item = NULL;

    while (1) {
        HASH_FIND_INT(*dict_chunk, &seq, item);
        if (item == NULL) break;

        fwrite(item->data, 1, item->len, fp);
        seq++;
    }

    delete_all_chunk(dict_chunk);
    fclose(fp);
}

int parse_packet(FILE* fp, Chunk** dict_chunk) {
    int count = 0;
    int seq = 0, len = 0, tmp = 0;

    for (int i = 0; i < 4; i++) {
        fread(&tmp, 1, 1, fp);
        seq = seq << 8;
        seq += tmp;
        printf("tmp: %.2x\n", tmp);
    }
    printf("chunk seq: %.2x\n", seq);
    printf("chunk seq: %d\n", seq);

    for (int i = 0; i < 2; i++) {
        fread(&tmp, 1, 1, fp);
        len = len << 8;
        len += tmp;
        printf("tmp: %.2x\n", tmp);
    }
    printf("chunk len: %.2x\n", len);
    printf("chunk len: %d\n", len);

    char* data = malloc(len);
    for (int i = 0; i < len; i++) {
        fread(&tmp, 1, 1, fp);
        data[i] = tmp;
        // printf("%02x ", data[i]);
        // if (count++ >= 15) {
        //     printf("\n");
        //     count = 0;
        // }
    }

    add_chunk(dict_chunk, seq, len, data);
    return 0;
}

// reassemble
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

Chunk* find_chunk(Chunk** dict_chunks, int key) {
    Chunk* found;
    HASH_FIND_INT(*dict_chunks, &key, found);
    return found;
}

void print_chuncks(Chunk** dict_chunks) {
    Chunk* item;

    for (item = *dict_chunks; item != NULL; item = item->hh.next) {
        printf("chunck seq: %d\n", item->seq);
        int count = 0;
        const int line_of_bytes = 15;
        const int bytes_to_print = 32;
        for (int i = 0; i < bytes_to_print; i++) {
            if (item->data[i] == 0) break;
            printf("%.2x ", item->data[i]);
            if (count++ >= line_of_bytes) {
                count = 0;
                printf("\n");
            }
        }
    }
}

void delete_chunk(Chunk** dict_chunks, Chunk* chuck_to_del) {
    HASH_DEL(*dict_chunks, chuck_to_del);
    free(chuck_to_del->data);
    chuck_to_del->data = NULL;
    free(chuck_to_del);
    chuck_to_del = NULL;
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

size_t got_data(char* buffer, size_t itemsize, size_t nitems, void* ignored) {
    size_t bytes = itemsize * nitems;
    printf("New chunk %zu bytes\n", bytes);
    int count = 16, linenum = 0;
    int idx = 0;
    while (idx < nitems) {
        if (count++ >= 16) {
            printf("\n%d: ", linenum++);
            count = 0;
        }
        printf("%hhx ", buffer[idx++]);
    }
    printf("\n ---END--- \n");
    return bytes;
}