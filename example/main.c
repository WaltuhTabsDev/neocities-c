#include <stdio.h>

#include "../neocities.h"

#ifdef _WIN32
#include <windows.h>
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    // Initialize the library with your credentials
    struct Neocities* neo = neocities_init("username", "password");
    if (!neo) {
        fprintf(stderr, "Failed to initialize neocities\n");
        return 1;
    }

    // Example 1: Get site info and hits
    printf("Getting site info...\n");
    int hits = neocities_get_hits(neo, "mooncatc");
    if (hits >= 0) {
        printf("Site hits: %d\n", hits);
    } else {
        fprintf(stderr, "Failed to get hits\n");
    }
        // Get views
    int views = neocities_get_views(neo, "mooncatc");
    if (views >= 0) {
        printf("Site views: %d\n", views);
    }

    // Get tags
    size_t tag_count;
    char** tags = neocities_get_tags(neo, "mooncatc", &tag_count);
    if (tags) {
        printf("Site tags:\n");
        for (size_t i = 0; i < tag_count; i++) {
            printf("  - %s\n", tags[i]);
        }
        neocities_free_tags(tags, tag_count);
    }

    // Get creation date
    time_t created_at = neocities_get_created_at(neo, "mooncatc");
    if (created_at > 0) {
        char date_str[64];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", localtime(&created_at));
        printf("Site created on: %s\n", date_str);
    }
    // Example 2: Upload a file
    printf("\nUploading file...\n");
    struct UploadFile file = {
        .filename = "test.txt",
        .filepath = "test.txt"
    };
    if (neocities_upload(neo, &file, 1)) {
        printf("File uploaded successfully\n");
    } else {
        fprintf(stderr, "Failed to upload file\n");
    }

    // Example 3: List files
    printf("\nListing files...\n");
    char* files = neocities_list_files(neo, NULL);
    if (files) {
        printf("Files:\n%s\n", files);
        free(files);
    } else {
        fprintf(stderr, "Failed to list files\n");
    }

    // Cleanup
    neocities_cleanup(neo);
    return 0;
} 