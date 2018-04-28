#include <stdio.h>
#include "../src/ezxml.h"

int main(int argc, char **argv)
{
    char *s;
    ezxml_t xml = ezxml_parse_file("tests/test.xml");
    printf("%s\n", (s = ezxml_toxml(xml)));
    free(s);
    int i = fprintf(stderr, "%s", ezxml_error(xml));
    ezxml_free(xml);
    return i ? 1 : 0;
}
