#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <malloc.h>

#include "runtime_files.h"

typedef struct {
    char* buffer;
    size_t size;
} read_result_t;

read_result_t read_file(const char* const path) {
    read_result_t result;
    result.buffer = NULL;
    result.size = -1;

    FILE* input = fopen(path, "r");
    if (input == NULL) {
        fprintf(stderr, "%s unable to open: %s", __FUNCTION__, path);
        return result;
    }

    fseek(input, 0, SEEK_END);
    ssize_t input_size = ftell(input);
    result.buffer = calloc(input_size, sizeof(char));

    fseek(input, 0, SEEK_SET);
    result.size = fread(result.buffer, 1, input_size, input);
    if (result.size != input_size) {
        fprintf(stderr, "%s error incomplete read.", __FUNCTION__);
        free(result.buffer);
        result.buffer = NULL;
        return result;
    }

    fclose(input);
    return result;
}

bool copy_file_to_fd(int output_fd, const char* const path) {
    read_result_t read_result = read_file(path);
    if (read_result.buffer != NULL) {
        write(output_fd, read_result.buffer, read_result.size);
        free(read_result.buffer);
        return 0;
    } else {
        return 1;
    }
}

int export_file_to_shm(const char* const source_path, const char* const target_path) {
    // shm_open doesn't survive over exec(), so we _have to_ keep this process alive and create a child for the runtime
    int writable_fd = shm_open(target_path, O_WRONLY | O_CREAT, 0555);

    if (writable_fd < 0) {
        fprintf(stderr, "shm_open failed (writable): %s\n", strerror(errno));
        return -1;
    }

    // open file read-only before unlinking the file, this is the fd we return later
    // otherwise we'll end up with ETXTBSY when trying to exec() it
    int readable_fd = shm_open(target_path, O_RDONLY, 0);

    if (readable_fd < 0) {
        fprintf(stderr, "shm_open failed (read-only): %s\n", strerror(errno));
        return -1;
    }

    if (copy_file_to_fd(writable_fd, source_path) != 0) {
        fprintf(stderr, "failed to copy and patch runtime\n");
        close(writable_fd);
        return -1;
    }

    // close writable fd and return readable one
    close(writable_fd);
    return readable_fd;
}
