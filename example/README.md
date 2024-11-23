# Neocities Library Example

This example demonstrates how to use the neocities library.

## Building

1. First, build the neocities library:
   ```bash
   cd ..
   build.bat dll
   ```

2. Then build this example:
   ```bash
   build.bat
   ```

## Running

1. Update the credentials in `main.c` with your neocities username and password
2. Run the example:
   ```bash
   build\example.exe
   ```

## What the Example Does

1. Initializes the neocities library with your credentials
2. Gets the hit count for your site
3. Uploads a test file
4. Lists all files on your site
5. Cleans up and exits

## Note

Make sure you have:
1. Built the neocities library as a DLL first
2. Have libcurl properly installed
3. Updated the credentials in main.c
4. Created a test.txt file if you want to test the upload functionality 