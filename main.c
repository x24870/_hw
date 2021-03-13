#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

size_t got_data(char *, size_t, size_t, void*);

int main() {
    CURL *curl;
    FILE *fp;
    int result;

    fp = fopen("downloaded_file", "wb");
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "test.jigentec.com:49152");
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    result = curl_easy_perform(curl);

    if (result == CURLE_OK) {
        printf("Download successful\n");
    } else {
        printf("ERROR: %s\n", curl_easy_strerror(result));
    }

    fclose(fp);
    curl_easy_cleanup(curl);
}

size_t got_data(char *buffer, size_t itemsize, size_t nitems, void* ignored) {
    size_t bytes = itemsize * nitems;
    printf("New chunk %zu bytes\n", bytes);
    int count = 16, linenum = 0;
    int idx = 0;
    while(idx < nitems){
        if(count++ >= 16){
            printf("\n%d: ", linenum++);
            count = 0;
        }
        printf("%hhx ", buffer[idx++]);
    }
    printf("\n ---END--- \n");
    return bytes;
}