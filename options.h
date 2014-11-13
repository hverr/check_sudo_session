#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>

typedef struct {
    int specified;
    unsigned int value;
} options_uint_t;

typedef struct {
    int specified;
} options_flag_t;

typedef struct {
    options_flag_t help;
    options_uint_t warning, critical;
    options_uint_t only_check;
} options_t;

int parse_argv(options_t *opts, int argc, const char **argv);
void print_usage(FILE *out, const char *prog);

#endif /* OPTIONS_H */
