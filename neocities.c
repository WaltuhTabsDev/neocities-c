#include "neocities.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Structure to hold response data
struct ResponseData {
    char* data;
    size_t size;
};

// Callback function for writing response data
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct ResponseData* resp = (struct ResponseData*)userp;
    
    char* ptr = realloc(resp->data, resp->size + realsize + 1);
    if (!ptr) {
        return 0;
    }
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, realsize);
    resp->size += realsize;
    resp->data[resp->size] = 0;
    
    return realsize;
}

// Helper function for CURL requests
static bool perform_curl_request(CURL* curl, const char* url, curl_mime* form, 
                               struct ResponseData* resp, const char* userpwd) {
    CURLcode res;
    bool success = false;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (form) {
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
    }
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);
    
    // SSL Options
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Don't verify peer
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Don't verify hostname
    
    // Optional: Enable verbose output for debugging
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL Error: %s\n", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        if (strstr(resp->data, "invalid_auth") != NULL) {
            fprintf(stderr, "Authentication error: %s\n", resp->data);
        } else {
            success = true;
        }
    }

    return success;
}

struct Neocities* neocities_init(const char* username, const char* password) {
    struct Neocities* neo = malloc(sizeof(struct Neocities));
    if (neo) {
        neo->username = strdup(username);
        neo->password = strdup(password);
    }
    return neo;
}

void neocities_cleanup(struct Neocities* neo) {
    if (neo) {
        free(neo->username);
        free(neo->password);
        free(neo);
    }
}

bool neocities_upload(struct Neocities* neo, struct UploadFile* files, size_t file_count) {
    CURL* curl;
    curl_mime* form = NULL;
    bool success = false;
    struct ResponseData resp = {0};

    // Check if files exist before proceeding
    for (size_t i = 0; i < file_count; i++) {
        FILE* test_file = fopen(files[i].filepath, "rb");
        if (!test_file) {
            fprintf(stderr, "Error: File not found: %s\n", files[i].filepath);
            return false;
        }
        fclose(test_file);
    }

    curl = curl_easy_init();
    if (curl) {
        form = curl_mime_init(curl);
        
        for (size_t i = 0; i < file_count; i++) {
            curl_mimepart* field = curl_mime_addpart(form);
            curl_mime_name(field, files[i].filename);
            curl_mime_filedata(field, files[i].filepath);
        }

        char auth[256];
        snprintf(auth, sizeof(auth), "%s:%s", neo->username, neo->password);
        
        success = perform_curl_request(curl, "https://neocities.org/api/upload",
                                     form, &resp, auth);

        // Check the response for specific error messages
        if (success && resp.data) {
            if (strstr(resp.data, "error") != NULL) {
                fprintf(stderr, "Upload error: %s\n", resp.data);
                success = false;
            }
        }

        curl_mime_free(form);
        curl_easy_cleanup(curl);
    }

    free(resp.data);
    return success;
}

bool neocities_delete_files(struct Neocities* neo, char** filenames, size_t filename_count) {
    CURL* curl;
    curl_mime* form = NULL;
    bool success = false;
    struct ResponseData resp = {0};

    curl = curl_easy_init();
    if (curl) {
        form = curl_mime_init(curl);
        
        // Add each filename to the form
        for (size_t i = 0; i < filename_count; i++) {
            curl_mimepart* field = curl_mime_addpart(form);
            curl_mime_name(field, "filenames[]");
            curl_mime_data(field, filenames[i], CURL_ZERO_TERMINATED);
        }

        char auth[256];
        snprintf(auth, sizeof(auth), "%s:%s", neo->username, neo->password);

        success = perform_curl_request(curl, "https://neocities.org/api/delete",
                                     form, &resp, auth);

        curl_mime_free(form);
        curl_easy_cleanup(curl);
    }

    free(resp.data);
    return success;
}

char* neocities_get_info(struct Neocities* neo, const char* sitename) {
    CURL* curl;
    struct ResponseData resp = {0};
    char* response = NULL;

    curl = curl_easy_init();
    if (curl) {
        char* encoded_sitename = curl_easy_escape(curl, sitename, strlen(sitename));
        char url[512];
        snprintf(url, sizeof(url), "https://neocities.org/api/info?sitename=%s", encoded_sitename);

        char auth[256];
        snprintf(auth, sizeof(auth), "%s:%s", neo->username, neo->password);

        if (perform_curl_request(curl, url, NULL, &resp, auth)) {
            response = strdup(resp.data);
        }

        curl_free(encoded_sitename);
        curl_easy_cleanup(curl);
    }

    free(resp.data);
    return response;
}

char* neocities_list_files(struct Neocities* neo, const char* path) {
    CURL* curl;
    struct ResponseData resp = {0};
    char* response = NULL;

    curl = curl_easy_init();
    if (curl) {
        char url[512] = "https://neocities.org/api/list";
        
        if (path && *path) {
            char* encoded_path = curl_easy_escape(curl, path, strlen(path));
            snprintf(url, sizeof(url), "https://neocities.org/api/list?path=%s", encoded_path);
            curl_free(encoded_path);
        }

        char auth[256];
        snprintf(auth, sizeof(auth), "%s:%s", neo->username, neo->password);

        if (perform_curl_request(curl, url, NULL, &resp, auth)) {
            response = strdup(resp.data);
        }

        curl_easy_cleanup(curl);
    }

    free(resp.data);
    return response;
}

// Helper function to find a JSON field and extract its value
static const char* find_json_field(const char* json, const char* field) {
    char search_str[256];
    snprintf(search_str, sizeof(search_str), "\"%s\":", field);
    
    const char* pos = strstr(json, search_str);
    if (!pos) return NULL;
    
    return pos + strlen(search_str);
}

// Helper function to parse integer from JSON value
static int parse_json_int(const char* str) {
    while (*str && (*str == ' ' || *str == '\t')) str++; // Skip whitespace
    if (*str == ':') str++; // Skip colon if present
    while (*str && (*str == ' ' || *str == '\t')) str++; // Skip whitespace
    
    return atoi(str);
}

int neocities_get_hits(struct Neocities* neo, const char* sitename) {
    char* info_json = neocities_get_info(neo, sitename);
    int hits = -1;
    
    if (info_json) {
        // First check if the request was successful
        const char* success = find_json_field(info_json, "result");
        if (success && strstr(success, "\"success\"")) {
            // Find the info object
            const char* info = find_json_field(info_json, "info");
            if (info) {
                // Find the hits field within info
                const char* hits_str = find_json_field(info, "hits");
                if (hits_str) {
                    hits = parse_json_int(hits_str);
                }
            }
        }
        free(info_json);
    }
    
    return hits;
}

// Helper function to parse JSON array
static char** parse_json_array(const char* str, size_t* count) {
    *count = 0;
    char** result = NULL;
    const char* p = str;
    
    // Skip to opening bracket
    while (*p && *p != '[') p++;
    if (*p != '[') return NULL;
    p++;
    
    // Count elements and allocate array
    const char* tmp = p;
    while (*tmp && *tmp != ']') {
        if (*tmp == '"') (*count)++;
        tmp++;
    }
    if (*count == 0) return NULL;
    
    result = malloc(sizeof(char*) * (*count));
    if (!result) return NULL;
    
    // Parse each string
    size_t idx = 0;
    while (*p && *p != ']' && idx < *count) {
        // Skip to opening quote
        while (*p && *p != '"') p++;
        if (*p != '"') break;
        p++;
        
        // Find closing quote
        const char* end = p;
        while (*end && *end != '"') end++;
        if (*end != '"') break;
        
        // Copy string
        size_t len = end - p;
        result[idx] = malloc(len + 1);
        if (result[idx]) {
            strncpy(result[idx], p, len);
            result[idx][len] = '\0';
            idx++;
        }
        
        p = end + 1;
    }
    
    *count = idx;
    return result;
}

// Helper function to parse ISO 8601 date string
static time_t parse_iso8601_date(const char* date_str) {
    struct tm tm = {0};
    int year, month, day;
    
    // Skip the opening quote if present
    if (*date_str == '"') date_str++;
    
    // Parse YYYY-MM-DD format
    if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) == 3) {
        tm.tm_year = year - 1900;  // Years since 1900
        tm.tm_mon = month - 1;     // Months are 0-11
        tm.tm_mday = day;
        tm.tm_hour = 12;           // Set to noon to avoid DST issues
        
        return mktime(&tm);
    }
    
    return 0;
}

int neocities_get_views(struct Neocities* neo, const char* sitename) {
    char* info_json = neocities_get_info(neo, sitename);
    int views = -1;
    
    if (info_json) {
        const char* success = find_json_field(info_json, "result");
        if (success && strstr(success, "\"success\"")) {
            const char* info = find_json_field(info_json, "info");
            if (info) {
                const char* views_str = find_json_field(info, "views");
                if (views_str) {
                    views = parse_json_int(views_str);
                }
            }
        }
        free(info_json);
    }
    
    return views;
}

char** neocities_get_tags(struct Neocities* neo, const char* sitename, size_t* tag_count) {
    char* info_json = neocities_get_info(neo, sitename);
    char** tags = NULL;
    *tag_count = 0;
    
    if (info_json) {
        const char* success = find_json_field(info_json, "result");
        if (success && strstr(success, "\"success\"")) {
            const char* info = find_json_field(info_json, "info");
            if (info) {
                const char* tags_str = find_json_field(info, "tags");
                if (tags_str) {
                    tags = parse_json_array(tags_str, tag_count);
                }
            }
        }
        free(info_json);
    }
    
    return tags;
}

time_t neocities_get_created_at(struct Neocities* neo, const char* sitename) {
    char* info_json = neocities_get_info(neo, sitename);
    time_t created_at = 0;
    
    if (info_json) {
        const char* success = find_json_field(info_json, "result");
        if (success && strstr(success, "\"success\"")) {
            const char* info = find_json_field(info_json, "info");
            if (info) {
                const char* created_at_str = find_json_field(info, "created_at");
                if (created_at_str) {
                    created_at = parse_iso8601_date(created_at_str);
                }
            }
        }
        free(info_json);
    }
    
    return created_at;
}

void neocities_free_tags(char** tags, size_t tag_count) {
    if (tags) {
        for (size_t i = 0; i < tag_count; i++) {
            free(tags[i]);
        }
        free(tags);
    }
}