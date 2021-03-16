#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

#include "reassemble.h"

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
    fp = fopen("restored_file", "w");
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

    return 0;
}

int parse_packet(FILE* fp, Chunk** dict_chunk) {
    int seq = 0, len = 0, tmp = 0;

    for (int i = 0; i < 4; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        seq = seq << 8;
        seq += tmp;
        printf("tmp: %.2x\n", tmp);
    }
    printf("chunk seq: %.2x\n", seq);
    printf("chunk seq: %d\n", seq);

    for (int i = 0; i < 2; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        len = len << 8;
        len += tmp;
        printf("tmp: %.2x\n", tmp);
    }
    printf("chunk len: %.2x\n", len);
    printf("chunk len: %d\n", len);

    char* data = malloc(len);
    for (int i = 0; i < len; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        data[i] = tmp;
    }

    add_chunk(dict_chunk, seq, len, data);

    return 0;
}