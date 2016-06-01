/**
 * Run tests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "homfly.h"

#define BUFFER_SIZE 2048

/**
 * Test the library against given data
 *
 * usage: $TEST/test_example data.txt
 *
 * the data.txt must be formatted with 3 lines per test:
 *  1 - name
 *  2 - link info
 *  3 - expected output
 *
 * Each line can be at most 2048 characters.
 */
int main(int argc, char *argv[])
{
    char name[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    char test[BUFFER_SIZE];
    char *out;
    int i;
    FILE *f;

    if (argc != 2)
    {
        printf("must give exactly one file to test\n");
        return 2;
    }

    f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fclose(f);
        return 1;
    }

    while(fgets(name, sizeof(name), f) != NULL)
    {
        if(fgets(input, sizeof(input), f) == NULL)
        {
            fclose(f);
            printf("missing link data");
            return 3;
        }
        if(fgets(test, sizeof(test), f) == NULL)
        {
            fclose(f);
            printf("missing output data");
            return 3;
        }
        // File ends with \n, but we need to ignore it
        name[strlen(name)-1] = '\0';
        input[strlen(input)-1] = '\0';
        test[strlen(test)-1] = '\0';

        out = homfly_str(input);
        if(strcmp(test, out) != 0)
        {
            printf("error in computation: %s\n", name);
            printf("expected: %s\n", test);
            printf("got     : %s\n", out);
            fclose(f);
            return 1;
        }
    }

    printf("correct result\n");
    fclose(f);
    return 0;
}
