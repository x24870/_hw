#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    CURL *curl;
    FILE *fp;
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
}
