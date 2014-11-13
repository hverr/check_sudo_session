#ifndef NAGIOS_EXIT_H
#define NAGIOS_EXIT_H

#define EXIT_OK 0
#define EXIT_WARNING 1
#define EXIT_CRITICAL 2
#define EXIT_UNKNOWN 3

#include <stdarg.h>

void nagios_ok(const char *format, ...);
void nagios_warning(const char *format, ...);
void nagios_critical(const char *format, ...);
void nagios_unknown(const char *format, ...);

#endif /* NAGIOS_EXIT_H */
