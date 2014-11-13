#include "options.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <pwd.h>

void _parse_error_not_int_type(const char *flag) {
    fprintf(stderr, "error: getopt: %s requires an integer argument\n", flag);
}

void _parse_error_unspecified(char flag) {
    fprintf(stderr, "error: getopt: -%c is required\n", flag);
}

int _parse_uint_option(options_uint_t *option, const char *flag) {
    unsigned int v;

    if(sscanf(optarg, "%u", &v) != 1) {
        _parse_error_not_int_type(flag);
        return -1;
    }

    option->specified = 1;
    option->value = v;
    return 0;
}

int _parse_only_check_option(options_uint_t *option, const char *flag) {
    unsigned int v;
    struct passwd *pwd;
    
    if(sscanf(optarg, "%u", &v) == 1) {
        option->specified = 1;
        option->value = v;
        return 0;
    }
    
    pwd = getpwnam(optarg);
    if(pwd == NULL) {
        fprintf(stderr, "error: getopt: %s: user %s not found\n", flag, optarg);
        return -1;
    }

    option->specified = 1;
    option->value = (int)(pwd->pw_uid);
    return 0;
}

int parse_argv(options_t *opts, int argc, const char **argv) {
    int c;

    memset(opts, 0, sizeof(*opts));

    while((c = getopt(argc, (char * const *)argv, "hw:c:o:")) != -1) {
        switch(c) {
            case 'h':
                opts->help.specified = 1;
                break;

            case 'w':
                if(_parse_uint_option(&opts->warning, "-w")) {
                    return -1;
                }
                break;

            case 'c':
                if(_parse_uint_option(&opts->critical, "-c")) {
                    return -1;
                }
                break;

            case 'o':
                if(_parse_only_check_option(&opts->only_check, "-o")) {
                    return -1;
                }
                break;

            case '?':
                switch(optopt) {
                    case 'w':
                    case 'c':
                    case 'o':
                        fprintf(stderr, "error: getopt: -%c requires an "
                                "argument\n", optopt);
                        break;

                    default:
                        fprintf(stderr, "error: getopt: unkown option -%c\n",
                                optopt);
                        break;
                }
                return -1;
                break;
            
            default:
                return -1;
        }
    }

    if(opts->help.specified) {
        print_usage(stdout, argv[0]);
        exit(0);
    }
    
    if(!opts->warning.specified) {
        _parse_error_unspecified('w');
        return -1;
    } else if(!opts->critical.specified) {
        _parse_error_unspecified('c');
        return -1;
    }

    return 0;
}

void print_usage(FILE *out, const char *prog) {
    fprintf(out, "Usage: %s -w warning -c critical [-o user]\n", prog);
    fprintf(out, "\n");
    fprintf(out, "Options:\n");
    fprintf(out, "    -w warning\n");
    fprintf(out, "        The minimum amount of time the sudo session must\n");
    fprintf(out, "        running before a warning is generated.\n");
    fprintf(out, "\n");
    fprintf(out, "    -c critical\n");
    fprintf(out, "        The minimum amount of time the sudo session must\n");
    fprintf(out, "        running before a critical status is generated.\n");
    fprintf(out, "\n");
    fprintf(out, "    -o user\n");
    fprintf(out, "        (optional, not implemented) Only check sudo\n");
    fprintf(out, "        sessions running with an effective UID equal to\n");
    fprintf(out, "        that of the specified user or the user id.\n");
}
