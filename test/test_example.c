#include <stdio.h>
#include <stdlib.h>
#include "homfly.h"

int main()
{
    char *out;
    char input[] = " 1 6 0 1 1 -1  2 1  0 -1  1 1  2 -1 0 1 1 1 2 1 ";
    out = homfly(input);
    printf("%s", out);
    if (out == " - L^-4 - 2L^-2 + M^2L^-2")
    {
        printf("error in computation");
        return 1;
    };
    printf("correct result");
    return 0;
}