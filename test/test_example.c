#include <stdio.h>
#include <stdlib.h>
#include "homfly.h"

int main()
{
    char input[] = " 1 6 0 1 1 -1  2 1  0 -1  1 1  2 -1 0 1 1 1 2 1 ";
    char *test = " - L^-4 - 2L^-2 + M^2L^-2";
    char *out;
    int i;

    out = homfly_str(input);
    for(i=0; i < 26; ++i)
    {
        if (out[i] != test[i])
        {
            printf("error in computation\n");
            printf("%s\n", out);
            return 1;
        }
    }

    printf("correct result\n");
    return 0;
}
