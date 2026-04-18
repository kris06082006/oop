#include <string.h>
#include <ctype.h>
#include "check.h"

Err check_format_of_array(const char* str) {
    if (str == NULL) {
        return ERR_NOT_INIT;
    }
    
    size_t len = strlen(str);
    if (len == 0) {
        return ERR_EMPTY;
    }
    for (size_t i = 0; i < len; i++) {
        if ((str[i] < '0' || str[i] > '9') && str[i] != ',') {
            return ERR_GARBAGE;
        }
    }
    if (strstr(str, ",,") != NULL) {
        return ERR_GARBAGE;
    }
    if (str[0] == ',') {
        return ERR_GARBAGE;
    }
    if (str[len - 1] == ',') {
        return ERR_GARBAGE;
    }

    return ERR_OK;
}
