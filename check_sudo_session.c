#include <stdio.h>
#include <stdlib.h>

#include "process.h"

int main() {
    process_t **proc_list, *p;
    size_t num_procs, i;

    proc_list = process_copy_running(&num_procs);
    if(num_procs == 0) {
        printf("error: process_copy_running\n");
        free(proc_list);
        exit(2);
    }

    for(i = 0; i < num_procs; i++) {
        p = proc_list[i];
        printf("(%p, %d, %s, %lld)\n", p, p->pid, p->comm,
               (long long)p->running);
        free(p);
    }
    free(proc_list);

    return 0;
}
