#include "java_properties.h"

char* array_to_properties(const char* source, const char* name){
    
    char* copy = strdup(source);
    if (copy == NULL) return NULL;

    int capacity = 256;
    char* result = malloc(capacity);
    if (result == NULL) { free(copy); return NULL; } 
    result[0] = '\0';

    char *num = strtok(copy, ",");
    int index = 1;
    size_t current_len = 0;

    while (num) {

        int line_len = snprintf(NULL, 0, "%s.%d=%s\n", name, index, num);

        if (line_len < 0) {
            free(copy);
            free(result);
            return NULL;
        }

        if (current_len + line_len + 1 > capacity){
            capacity = (current_len + line_len + 1) * 2;
            char* new_result = realloc(result, capacity);

            if(new_result == NULL){
                free(copy);
                free(result);
                return NULL;
            }
            result = new_result;
        }
        current_len += snprintf(result + current_len, capacity - current_len, "%s.%d=%s\n", name, index++, num);
    
        num = strtok(NULL, ",");
    }

    free(copy);

    char* final_result = realloc(result, current_len + 1);
    if (final_result != NULL) { return final_result; }
    else { return NULL; }
}

char* properties_to_array(const char* properties) {

    int capacity = strlen(properties) + 1;
    char* result = malloc(capacity);
    if (result == NULL) return NULL;

    result[0] = '\0';
    
    const char* ptr = properties;
    
    while (*ptr) {
        if (*ptr == '=') {
            const char* start_num = ptr + 1;
            const char* end_num = strchr(start_num, '\n');
            
            if (end_num == NULL) {
                end_num = start_num + strlen(start_num);
            }

            int len = end_num - start_num;
            strncat(result, start_num, len);
            strcat(result, ",");
        }
        ptr++;
    }
    int len = strlen(result);
    result[len - 1] = '\0';

    char* final_result = realloc(result, len + 1);
    if (final_result != NULL) { return final_result; }
    else { free(result); return NULL; }
}
