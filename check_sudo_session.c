#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "process.h"
#include "nagios_exit.h"

void perform_check(options_t *options);

int main(int argc, const char **argv) {
    options_t options;

    if(parse_argv(&options, argc, argv) != 0) {
        print_usage(stderr, argv[0]);
        exit(EXIT_CRITICAL);
    }

    perform_check(&options);

    return 0;
}

void perform_check(options_t *options) {
    size_t num_procs = 0, i;
    process_t **list, *p;
    process_t *warning = NULL, *critical = NULL;

    list = process_copy_running(&num_procs);
    if(list == NULL || num_procs == 0) {
        nagios_critical("could not get running processes");
        return;
    }

    for(i = 0; i < num_procs; i++) {
        p = list[i];
        if(strcmp(p->comm, "sudo") != 0) {
            continue;
        }

        if((time_t)p->running > (time_t)options->critical.value) {
            if(critical == NULL || critical->running < p->running) {
                critical = p;
            }
        }

        if((time_t)p->running > (time_t)options->warning.value) {
            if(warning == NULL || warning->running < p->running) {
                warning = p;
            }
        }
    }

    if(critical) {
        nagios_critical("hanging session: %d seconds (pid: %d)",
                        (int)critical->running, (int)critical->pid);
    } else if(warning) {
        nagios_warning("hanging session: %d seconds (pid: %d)",
                       (int)warning->running, (int)warning->pid);
    } else {
        nagios_ok("no hanging sessions");
    }

    for(i= 0; i < num_procs; i++) {
        free(list[i]);
    }
    free(list);
}
