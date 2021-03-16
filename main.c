#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

#include "reassemble.h"

int fetch_file(const char*, const char*);

int main() {
    char url[] = "test.jigentec.com:49152";
    char filename[] = "downloaded_file";

    if (fetch_file(url, filename) < 0) exit(1);

    // define a dictionary to contain data chunk
    // key is sequence of the chunk
    // value is the data in the chunk
    Chunk* dict_chunk = NULL;
    if (parse_file(filename, &dict_chunk) == 0) {
        reassemble(&dict_chunk);
    } else {
        printf("Parsing packet failed, skip reassemble process\n");
    }

    delete_all_chunk(&dict_chunk);
}

int fetch_file(const char* url, const char* filename) {
    CURL* curl;
    FILE* fp;
    int result;

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("ERROR: Unable to open the file\n\t%s\n", filename);
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
        printf("ERROR: unable to download file\n\t%s\n", curl_easy_strerror(result));
        return -1;
    }

    printf("Download successfully\n");
    return 0;
}
