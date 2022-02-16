#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "runtime_files.h"

typedef struct {
    char* buffer;
    size_t size;
} buffer_t;

buffer_t read_file(const char* const path) {
    buffer_t result;
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
    buffer_t read_result = read_file(path);
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


int create_tmp_file(buffer_t const data) {
    char tmp_path[] = "/tmp/.appimage-bin-XXXXXX";
    int target_fd = mkostemp(tmp_path, O_EXCL | O_CREAT | O_RDWR);
    if (target_fd < 0) {
        fprintf(stderr, "mkstemp failed: %s\n", strerror(errno));
        return -1;
    }

    int result_fd = open(tmp_path, O_RDONLY);
    fchmod(target_fd, S_IRUSR | S_IXUSR);
    unlink(tmp_path);

    write(target_fd, data.buffer, data.size);
    close(target_fd);

    return result_fd;
}

void patch_bin(buffer_t data) {
}

int exec_bin(int fd, char* const argv[], char* const envp[]) {
    int ret = fexecve(fd, argv, envp);
    if (ret != 0) {
        fprintf(stderr, "fexecve failed: %s\n", strerror(errno));
        return -1;
    }
    return ret == 0;
}

bool exec_portable_bin(char const* const path, char* const argv[], char* const envp[]) {
    buffer_t data = read_file(path);
    if (data.buffer == NULL)
        return false;

    patch_bin(data);

    int bin_fd = create_tmp_file(data);
    if (bin_fd < 0)
        return false;

    return exec_bin(bin_fd, argv, envp);
}