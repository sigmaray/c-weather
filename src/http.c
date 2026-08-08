#include "http.h"

#include <curl/curl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char *data;
    size_t size;
} WriteBuffer;

/* curl CURLOPT_WRITEFUNCTION requires non-const char* */
static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    WriteBuffer *buf = userdata;
    size_t total = size * nmemb;
    char *p = realloc(buf->data, buf->size + total + 1);
    if (!p) {
        return 0;
    }
    buf->data = p;
    memcpy(buf->data + buf->size, ptr, total);
    buf->size += total;
    buf->data[buf->size] = '\0';
    return total;
}

void http_global_init(void) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void http_global_cleanup(void) {
    curl_global_cleanup();
}

void http_response_free(HttpResponse *resp) {
    if (!resp) {
        return;
    }
    free(resp->data);
    resp->data = NULL;
    resp->size = 0;
}

bool http_get(const char *url, const char *user_agent, HttpResponse *out) {
    memset(out, 0, sizeof(*out));
    out->status_code = -1;

    CURL *curl = curl_easy_init();
    if (!curl) {
        snprintf(out->error, sizeof(out->error), "curl_easy_init failed");
        return false;
    }

    WriteBuffer buf = {0};
    struct curl_slist *headers = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    if (user_agent && user_agent[0]) {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
    } else {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "WeatherApp/1.0");
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    CURLcode rc = curl_easy_perform(curl);
    clock_gettime(CLOCK_MONOTONIC, &end);

    out->duration_ms = (end.tv_sec - start.tv_sec) * 1000L +
                       (end.tv_nsec - start.tv_nsec) / 1000000L;

    if (rc != CURLE_OK) {
        snprintf(out->error, sizeof(out->error), "%s", curl_easy_strerror(rc));
        free(buf.data);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    out->status_code = (int)code;
    out->data = buf.data;
    out->size = buf.size;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return true;
}

bool url_encode(const char *src, char *buf, size_t bufsize) {
    static const char *hex = "0123456789ABCDEF";
    size_t j = 0;
    for (size_t i = 0; src[i]; i++) {
        unsigned char c = (unsigned char)src[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
            c == '~') {
            if (j + 1 >= bufsize) {
                return false;
            }
            buf[j++] = (char)c;
        } else if (c == ' ') {
            if (j + 1 >= bufsize) {
                return false;
            }
            buf[j++] = '+';
        } else {
            if (j + 3 >= bufsize) {
                return false;
            }
            buf[j++] = '%';
            buf[j++] = hex[c >> 4];
            buf[j++] = hex[c & 0xF];
        }
    }
    if (j >= bufsize) {
        return false;
    }
    buf[j] = '\0';
    return true;
}
