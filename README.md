# libhomfly
## Library to compute the homfly polynomial of a link.

This is basically a conversion of the program writen by Robert J Jenkins Jr into a shared library. It accepts as entry a character string, formated in the same way as the input files that the original code (see below). The returned value is the string that the original value did print on screen.

The modifications done to the original program are:

* modified input/output to allow being called as a shared library
* the library now does not write any output to stdout
* memory allocation moved to boehmgc to prevent memory leaks

### Dependencies

* standard c library
* boehmgc 

### Instalation

Just running

make 

should compile the library. Then just copy the header homfly.h to your headers directory (in linux, it is usually /usr/include/) and the file libhomfly.so to your library directory (in linux /usr/lib/).

### Example of usage

This simple program can be used as an example on how to use the library. In particular, it computes the homfly polynomial of the trefoil knot.

``````````````````````````````````````````````````````````````````````````````````````````
#include <stdio.h>
#include <stdlib.h>
#include "homfly.h"
#include <gc.h>

int main()
{
    char *out;
    char input[] = " 1 6 0 1 1 -1  2 1  0 -1  1 1  2 -1 0 1 1 1 2 1 ";
    out = homfly(input);
    printf("%s", out);
    //free(out);
    return 0;
}
``````````````````````````````````````````````````````````````````````````````````````````

Note that, in order to compile it, you need to have the boehm garbage collector library available, and compile the program with the -lgc option.

