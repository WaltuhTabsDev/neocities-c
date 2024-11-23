# Neocities C Library Documentation

A C library for interacting with the Neocities API. This library provides functions for uploading files, managing site content, and retrieving site statistics.

## Table of Contents
- [Installation](#installation)
- [Basic Usage](#basic-usage)
- [API Reference](#api-reference)
  - [Initialization](#initialization)
  - [File Operations](#file-operations)
  - [Site Information](#site-information)
  - [Cleanup](#cleanup)
- [Data Structures](#data-structures)
- [Error Handling](#error-handling)
- [Examples](#examples)

## Installation

1. Include the library headers:
```c
#include "neocities.h"
```

2. Link against the library:
```bash
gcc -o your_program your_program.c -lneocities -lcurl
```

## Basic Usage

```c
// Initialize the library
struct Neocities* neo = neocities_init("username", "password");
if (!neo) {
    fprintf(stderr, "Failed to initialize\n");
    return 1;
}

// Use the library functions
// ...

// Cleanup when done
neocities_cleanup(neo);
```

## API Reference

### Initialization

#### `struct Neocities* neocities_init(const char* username, const char* password)`
Initializes the library with your Neocities credentials.
- **Parameters:**
  - `username`: Your Neocities username
  - `password`: Your Neocities password
- **Returns:** Pointer to Neocities structure or NULL on failure

### File Operations

#### `bool neocities_upload(struct Neocities* neo, struct UploadFile* files, size_t file_count)`
Uploads one or more files to your Neocities site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `files`: Array of UploadFile structures
  - `file_count`: Number of files to upload
- **Returns:** true on success, false on failure

#### `bool neocities_delete_files(struct Neocities* neo, char** filenames, size_t filename_count)`
Deletes one or more files from your Neocities site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `filenames`: Array of filenames to delete
  - `filename_count`: Number of files to delete
- **Returns:** true on success, false on failure

#### `char* neocities_list_files(struct Neocities* neo, const char* path)`
Lists files in the specified directory.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `path`: Directory path (NULL for root)
- **Returns:** JSON string containing file list (must be freed) or NULL on failure

### Site Information

#### `char* neocities_get_info(struct Neocities* neo, const char* sitename)`
Gets information about a Neocities site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `sitename`: Site to get information about
- **Returns:** JSON string containing site info (must be freed) or NULL on failure

#### `int neocities_get_hits(struct Neocities* neo, const char* sitename)`
Gets the number of hits for a site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `sitename`: Site to get hits for
- **Returns:** Number of hits or -1 on failure

#### `int neocities_get_views(struct Neocities* neo, const char* sitename)`
Gets the number of views for a site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `sitename`: Site to get views for
- **Returns:** Number of views or -1 on failure

#### `char** neocities_get_tags(struct Neocities* neo, const char* sitename, size_t* tag_count)`
Gets the tags for a site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `sitename`: Site to get tags for
  - `tag_count`: Pointer to store number of tags
- **Returns:** Array of strings containing tags (must be freed with neocities_free_tags) or NULL on failure

#### `time_t neocities_get_created_at(struct Neocities* neo, const char* sitename)`
Gets the creation date of a site.
- **Parameters:**
  - `neo`: Neocities structure from init
  - `sitename`: Site to get creation date for
- **Returns:** Unix timestamp of creation date or 0 on failure

### Cleanup

#### `void neocities_cleanup(struct Neocities* neo)`
Cleans up and frees resources.
- **Parameters:**
  - `neo`: Neocities structure to cleanup

#### `void neocities_free_tags(char** tags, size_t tag_count)`
Frees tag array returned by neocities_get_tags.
- **Parameters:**
  - `tags`: Array of tags to free
  - `tag_count`: Number of tags

## Data Structures

### `struct Neocities`
Holds authentication information.
```c
struct Neocities {
    char* username;
    char* password;
};
```

### `struct UploadFile`
Holds information about a file to upload.
```c
struct UploadFile {
    char* filename;  // Name to use on the server
    char* filepath;  // Local path to the file
};
```

## Error Handling

Most functions return either:
- `NULL` or `-1` on failure
- A valid pointer or non-negative number on success

Error messages are printed to stderr.

## Examples

### Upload a File
```c
struct UploadFile file = {
    .filename = "index.html",
    .filepath = "local/path/to/index.html"
};

if (neocities_upload(neo, &file, 1)) {
    printf("Upload successful\n");
} else {
    fprintf(stderr, "Upload failed\n");
}
```

### Get Site Statistics
```c
int views = neocities_get_views(neo, "sitename");
if (views >= 0) {
    printf("Site views: %d\n", views);
}

size_t tag_count;
char** tags = neocities_get_tags(neo, "sitename", &tag_count);
if (tags) {
    printf("Site tags:\n");
    for (size_t i = 0; i < tag_count; i++) {
        printf("  - %s\n", tags[i]);
    }
    neocities_free_tags(tags, tag_count);
}
```