#ifndef _GMYSQL_UTILS_H
#define _GMYSQL_UTILS_H

#include <glib.h>
#include <errno.h>

int gmysql_count_noascii_character (const unsigned char * string);

gchar * gmysql_alloc_iconv(GIConv icv, const char * source);

#endif /* _GMYSQL_UTILS_H */
