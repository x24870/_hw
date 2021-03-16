#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>

typedef struct chunk {
    int seq;
    char* data;
    UT_hash_handle hh;
} Chunk;

int add_chunk(Chunk**, int, char*);
Chunk* find_chunk(Chunk**, int);
void print_chuncks(Chunk**);
void delete_chunk(Chunk**, Chunk*);

size_t got_data(char*, size_t, size_t, void*);

int main() {
    // CURL* curl;
    // FILE* fp;
    // int result;

    // fp = fopen("downloaded_file", "wb");
    // curl = curl_easy_init();

    // curl_easy_setopt(curl, CURLOPT_URL, "test.jigentec.com:49152");
    // // curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);
    // curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    // result = curl_easy_perform(curl);

    // if (result == CURLE_OK) {
    //     printf("Download successful\n");
    // } else {
    //     printf("ERROR: %s\n", curl_easy_strerror(result));
    // }

    // fclose(fp);
    // curl_easy_cleanup(curl);

    //reassemble
    Chunk* dict_chunks = NULL;

    char data[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    add_chunk(&dict_chunks, 0, data);
    print_chuncks(&dict_chunks);
    Chunk* c = find_chunk(&dict_chunks, 0);
    delete_chunk(&dict_chunks, c);
}

int add_chunk(Chunk** dict_chunks, int seq, char* data) {
    Chunk* new_chunk;

    // check if the key is used
    HASH_FIND_INT(*dict_chunks, &seq, new_chunk);
    if (new_chunk != NULL) {
        return -1;
    }

    new_chunk = malloc(sizeof(Chunk));
    new_chunk->seq = seq;
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
            if(item->data[i] == 0) break;
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
    free(chuck_to_del);
    chuck_to_del = NULL;
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