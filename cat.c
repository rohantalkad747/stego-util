
#include <stdio.h>

void fcpy(FILE *, FILE *);

int cat(int argc, char *argv[])
{
    FILE *f;
    if (argc == 1) {
        fcpy(stdin, stdout);
        putc('\n', stdout);
    } else {
        while (--argc) {
            if ((f = fopen(*++argv, "r")) != NULL) {
                fcpy(f, stdout);
                fclose(f);
                putc('\n', stdout);
            }
        }
    }
    return 0;
}

void fcpy(FILE *ipf, FILE *opf)
{
    int c;
    while ((c = getc(ipf)) != EOF)
        putc(c, opf);
}

