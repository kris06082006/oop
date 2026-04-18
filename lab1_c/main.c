#include "java_properties.h"
#include "io.h"
#include "error.h"
#include "check.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    char* array = NULL;
    char* name = NULL;
    char* java_properties = NULL;
    char* integer_array = NULL;
    char* err;
    Err status = ERR_OK;

    printf("Input name of integer array: ");
    status = readline(stdin, &name);

    err = error_message(status);
    printf("%s",  err);
    if(status != ERR_OK) {
        goto clean;
    }
    
    printf("Input integer array (formst: \"1,2,3...\"): ");
    status = readline(stdin, &array);

    err = error_message(status);
    printf("%s",  err);
    if(status != ERR_OK) {
        goto clean;
    }

    status = check_format_of_array(array);

    err = error_message(status);
    printf("%s",  err);
    if(status != ERR_OK) {
        goto clean;
    }

    java_properties = array_to_properties(array, name);

    if (java_properties == NULL) {
        printf("Couldn't convert array to java properties...");
        goto clean;
    }

    printf("Your array as java properties:\n%s", java_properties);

    integer_array = properties_to_array(java_properties);

    if (integer_array == NULL) {
        printf("Couldn't convert array to java properties...");
        goto clean;
    }

    printf("Your array as java properties: %s", integer_array);

clean:
    free(array);
    free(name);
    free(java_properties);
    free(integer_array);

    return (status == ERR_OK) ? 0 : 1;
}

