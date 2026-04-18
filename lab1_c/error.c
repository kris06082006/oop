#include "error.h"

char *error_message(Err status){
    static char* array_error[13] = {
        "End of file.\n",
        "Error of memory allocation.\n",
	    "Structure is empty.\n",
	    "Error of reading from file.\n",
	    "You enter garbage.\n",
	    "Elements not found.\n",
	    "Error of opening file.\n",
	    "Error of writing to file.\n",
        "Structure is not initialized.\n",
        "Structure already is initialized.\n",
        "Failed to execute system command.\n",
        "Overflow error.\n",
        "",
    };
    return array_error[status];

}

