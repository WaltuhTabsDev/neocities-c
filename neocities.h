#ifndef NEOCITIES_H
#define NEOCITIES_H

#include <curl/curl.h>
#include <stdbool.h>
#include <time.h>

// Structure to hold credentials
struct Neocities {
    char* username;
    char* password;
};

// Structure to hold file upload info
struct UploadFile {
    char* filename;
    char* filepath;
};

// Initialize and cleanup functions
struct Neocities* neocities_init(const char* username, const char* password);
void neocities_cleanup(struct Neocities* neo);

// API functions
bool neocities_upload(struct Neocities* neo, struct UploadFile* files, size_t file_count);
bool neocities_delete_files(struct Neocities* neo, char** filenames, size_t filename_count);
char* neocities_get_info(struct Neocities* neo, const char* sitename);
char* neocities_list_files(struct Neocities* neo, const char* path);
int neocities_get_hits(struct Neocities* neo, const char* sitename);
int neocities_get_views(struct Neocities* neo, const char* sitename);
char** neocities_get_tags(struct Neocities* neo, const char* sitename, size_t* tag_count);
time_t neocities_get_created_at(struct Neocities* neo, const char* sitename);
void neocities_free_tags(char** tags, size_t tag_count);

#endif
