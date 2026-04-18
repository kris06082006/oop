#include "io.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Err readline(FILE* in, char** dst) {
    char buf[256];

    size_t size = 1;
    char* res = strdup("");
    if (res == NULL) {
        return ERR_MEM;
    }

    int cnt;

    do {
        cnt = fscanf(in, "%255[^\n]", buf);
        if (cnt < 0) {
            free(res);
            return ERR_EOF;
        }
        if (cnt > 0) {
            size += strlen(buf);

            char* tmp = realloc(res, size);
            if (tmp == NULL) {
                free(res);
                return ERR_MEM;
            }
            res = tmp;
            strcat(res, buf);
        }
    } while (cnt != 0);

    int c;
    while ((c = fgetc(in)) != '\n' && c != EOF);

    *dst = res;
    return ERR_OK;
}
