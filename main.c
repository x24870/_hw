#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

#include "reassemble.h"

int parse_file_to_dict(const char*, Chunk**);
int parse_packet(FILE*, Chunk**);
int fetch_file(const char*, const char*);

int main() {
    char url[] = "test.jigentec.com:49152";
    char filename[] = "downloaded_file";
    int ret;

    ret = fetch_file(url, filename);
    if (ret < 0) {
        exit(1);
    }

    //parse file
    Chunk* dict_chunks = NULL;
    parse_file_to_dict(filename, &dict_chunks);
}

int fetch_file(const char* url, const char* filename) {
    CURL* curl;
    FILE* fp;
    int result;

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("ERROR: Unable to open the file: %s\n", filename);
        return -1;
    }
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    result = curl_easy_perform(curl);
    fclose(fp);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        printf("ERROR: %s\n", curl_easy_strerror(result));
        return -1;
    }
    
    printf("Download successfully\n");
    return 0;
}

// process file
int parse_file_to_dict(const char* filename, Chunk** dict_chunk) {
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
    }

    for (int i = 0; i < 2; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        len = len << 8;
        len += tmp;
    }

    char* data = malloc(len);
    for (int i = 0; i < len; i++) {
        if ((fread(&tmp, 1, 1, fp)) != 1)
            return -1;
        data[i] = tmp;
    }

    add_chunk(dict_chunk, seq, len, data);

    return 0;
}