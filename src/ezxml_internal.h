#ifndef _EZXML_INTERNAL_H
#define _EZXML_INTERNAL_H

#include "ezxml.h"

#define EZXML_BUFSIZE 1024  // size of internal memory buffers
#define EZXML_WS "\t\r\n "  // whitespace
#define EZXML_ERRL 128      // maximum error string length

typedef struct ezxml_root *ezxml_root_t;
struct ezxml_root {        // additional data for the root tag
    struct ezxml xml;      // is a super-struct built on top of ezxml struct
    ezxml_t cur;           // current xml tree insertion point
    char *m;               // original xml string
    size_t len;            // length of allocated memory for mmap, -1 for malloc
    char *u;               // UTF-8 conversion of string if original was UTF-16
    char *s;               // start of work area
    char *e;               // end of work area
    char **ent;            // general entities (ampersand sequences)
    char ***attr;          // default attributes
    char ***pi;            // processing instructions
    short standalone;      // non-zero if <?xml standalone="yes"?>
    char err[EZXML_ERRL];  // error string
};

#endif
