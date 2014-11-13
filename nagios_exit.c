#include "nagios_exit.h"

#include <stdio.h>
#include <stdlib.h>

#define nagios_print_format(format_arg) {\
    va_list l;\
    va_start(l, format_arg);\
    vprintf(format_arg, l);\
    va_end(l);\
}

#define nagios_print_status(status, format) {\
    if(!format || *format == 0) {\
        printf(status);\
    } else {\
        printf(status" - ");\
    }\
}

void nagios_ok(const char *format, ...) {
    nagios_print_status("OK", format);
    nagios_print_format(format);
    printf("\n");
    exit(EXIT_OK);
}

void nagios_warning(const char *format, ...) {
    nagios_print_status("WARNING", format);
    nagios_print_format(format);
    printf("\n");
    exit(EXIT_WARNING);
}

void nagios_critical(const char *format, ...) {
    nagios_print_status("CRITICAL", format);
    nagios_print_format(format);
    printf("\n");
    exit(EXIT_CRITICAL);
}

void nagios_unknown(const char *format, ...) {
    nagios_print_status("UNKNOWN", format);
    nagios_print_format(format);
    printf("\n");
    exit(EXIT_UNKNOWN);
}
