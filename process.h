#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <time.h>

typedef struct {
    pid_t pid;
    char comm[256];
    time_t running;
} process_t;

process_t *process_create_from_directory(const char *directory);
process_t *process_create_from_pid(const char *pid);
process_t *process_create_from_file(const char *file);
process_t **process_copy_running(size_t *num_proc);

#endif /* PROCESS_H */
