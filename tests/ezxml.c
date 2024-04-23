#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../src/ezxml.h"

#ifndef EZXML_NOMMAP
#include <sys/mman.h>
#endif  // EZXML_NOMMAP

#include "../src/ezxml_internal.h"

// A wrapper for ezxml_parse_str() that accepts a file descriptor. First
// attempts to mem map the file. Failing that, reads the file into memory.
// Returns NULL on failure.
ezxml_t ezxml_parse_fd(int fd)
{
    ezxml_root_t root;
    struct stat st;
    size_t l;
    void *m;

    if (fd < 0)
        return NULL;
    fstat(fd, &st);

#ifndef EZXML_NOMMAP
    l = (st.st_size + sysconf(_SC_PAGESIZE) - 1) & ~(sysconf(_SC_PAGESIZE) - 1);
    if ((m = mmap(NULL, l, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) !=
        MAP_FAILED) {
        posix_madvise(m, l,
                      POSIX_MADV_SEQUENTIAL);  // optimize for sequential access
        root = (ezxml_root_t) ezxml_parse_str(m, st.st_size);
        if (!root) {
            munmap(m, l);
            return NULL;
        }
        posix_madvise(m, root->len = l,
                      POSIX_MADV_NORMAL);  // put it back to normal
    } else {                               // mmap failed, read file into memory
#endif                                     // EZXML_NOMMAP
        l = read(fd, m = malloc(st.st_size), st.st_size);
        root = (ezxml_root_t) ezxml_parse_str(m, l);
        if (!root) {
            free(m);
            return NULL;
        };
        root->len = -1;  // so we know to free s in ezxml_free()
#ifndef EZXML_NOMMAP
    }
#endif  // EZXML_NOMMAP
    return &root->xml;
}

// a wrapper for ezxml_parse_fd that accepts a file name
ezxml_t ezxml_parse_file(const char *file)
{
    int fd = open(file, O_RDONLY, 0);
    ezxml_t xml = ezxml_parse_fd(fd);

    if (fd >= 0)
        close(fd);
    return xml;
}

int main(int argc, char **argv)
{
    ezxml_t xml = ezxml_parse_file("tests/test.xml");
    char *s = ezxml_toxml(xml);
    if (s) {
        printf("%s\n", s);
        free(s);
    }
    int i = fprintf(stderr, "%s", ezxml_error(xml));
    ezxml_free(xml);
    return i ? 1 : 0;
}
