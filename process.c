#include "process.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#define PROCESS_PROC_DIR ("/proc")
#define PROCESS_STAT_FILE ("stat")

int _process_read_file(process_t *proc, FILE *fh);
int _process_read_next_num(FILE *fh, long long *value);
char *_process_copy_next_string(FILE *fh);
int _process_skip_next_string(FILE *fh);

long long _process_get_uptime(void);

process_t *process_create_from_directory(const char *directory) {
    process_t *p;
    char *file;
    size_t dir_len;

    dir_len = strlen(directory);

    file = malloc(dir_len + strlen(PROCESS_STAT_FILE) + 2);
    strcpy(file, directory);
    file[dir_len] = '/';
    dir_len++;
    strcpy(&(file[dir_len]), PROCESS_STAT_FILE);

    p = process_create_from_file(file);
    free(file);
    return p;
}

process_t *process_create_from_pid(const char *pid) {
    process_t *p;
    char *fullpath;

    fullpath = malloc(strlen(PROCESS_PROC_DIR) + strlen(pid) +
                      strlen(PROCESS_STAT_FILE) + 3);
    sprintf(fullpath, "%s/%s/%s", PROCESS_PROC_DIR, pid, PROCESS_STAT_FILE);

    p = process_create_from_file(fullpath);
    free(fullpath);
    return p;
}

process_t *process_create_from_file(const char *file) {
    process_t *p;
    FILE *fh;

    fh = fopen(file, "r");
    if(fh == NULL) {
        return NULL;
    }

    p = calloc(1, sizeof(*p));
    if(_process_read_file(p, fh) != 0) {
        free(p);
        fclose(fh);
        return NULL;
    }
    fclose(fh);
    return p;
}

process_t **process_copy_running(size_t *out_num_proc) {
    DIR *dir;
    struct dirent *ent;
    process_t **procs = NULL, *p;
    size_t num_proc = 0;

    dir = opendir(PROCESS_PROC_DIR);
    if(dir == NULL) {
        return NULL;
    }

    for(ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        int pid;
        if(sscanf(ent->d_name, "%d", &pid) == 1) {
            p = process_create_from_pid(ent->d_name);
            if(p != NULL && p->pid == pid) {
                procs = realloc(procs, sizeof(*procs) * (num_proc + 1));
                procs[num_proc] = p;
                num_proc++;
            } else if(p != NULL) {
                free(p);
            }
        }
    }

    closedir(dir);

    if(out_num_proc) {
        *out_num_proc = num_proc;
    }

    return procs;
}

int _process_read_file(process_t *proc, FILE *fh) {
    long long lld_value;
    char *str_value;
    time_t uptime;
    int i;

    _process_read_next_num(fh, &lld_value); /* pid */
    proc->pid = (pid_t)lld_value;

    str_value = _process_copy_next_string(fh); /* comm */
    strncpy(proc->comm, str_value, sizeof(proc->comm));
    free(str_value);

    /* state, ppid, sid, tty_nr, tty_pgrp, flags,
     * min_flt, cmin_flt, maj_flt, cmaj_flt,
     * utime, stimev, cutime, cstime, priority, nicev
     * num_threads, it_real_value */
    for(i = 0; i < 19; i++) {
        _process_skip_next_string(fh);
    }

    if(_process_read_next_num(fh, &lld_value) != 0) {
        fprintf(stderr, "error: _process_read_next_num: start_time\n");
        return -1;
    }

    /* not interested in what follow */
    /* convert start time to time running*/
    lld_value /= sysconf(_SC_CLK_TCK);
    uptime = (time_t)_process_get_uptime();
    proc->running = (time_t)uptime - (time_t)lld_value;

    return 0;
}

int _process_read_next_num(FILE *fh, long long *out_value) {
    return fscanf(fh, "%lld ", out_value) == 1 ? 0 : -1;
}

char *_process_copy_next_string(FILE *fh) {
    char *out = NULL;
    size_t out_size = 0;
    int ch;
    do {
        ch = fgetc(fh);
        if(ch == ' ' || ch == EOF) {
            break;
        }

        out = realloc(out, out_size + 1);
        out[out_size] = (char)ch;
        out_size++;
    } while(1);

    if(out == NULL) {
        return NULL;
    }

    out = realloc(out, out_size + 1);
    out[out_size] = 0;
    return out;
}

int _process_skip_next_string(FILE *fh) {
    int ch;
    do {
        ch = fgetc(fh);
    } while(ch != ' ' && ch != EOF);
    return 0;
}

long long _process_get_uptime(void) {
    FILE *procuptime;
    long long sec, ssec;
    procuptime = fopen("/proc/uptime", "r");
    fscanf(procuptime, "%lld.%lld ", &sec, &ssec);
    fclose(procuptime);
    return sec;
}
