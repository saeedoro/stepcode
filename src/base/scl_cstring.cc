#include "scl_cstring.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <iostream>

/**
    Function: scl_strcpy_s
    Description:
        Wrapper for strcpy function, checks for overflow on dest buffer.
*/
char * scl_strcpy_s( char * dest, size_t dest_size, const char *src ) {
    char * ptr;
    size_t available;

    // Initialize locals
    ptr = dest;
    available = dest_size;

    // Copy src to dest until src is '\0' or dest is full
    while ((*ptr++ = *src++) != 0 && --available > 0) {
    }

    // Check if dest was too small
    if (available == 0) {
        std::cerr << "SCL internal error (scl_strcpy_s): dest buffer too small." << std::endl;
        exit(1);
    }

    // Terminate dest with '\0'
    *ptr++ = '\0';

    return dest;
}

/**
    Function: scl_strncpy_s
    Description:
        Wrapper for strncpy function, checks for overflow on dest buffer.
*/
char * scl_strncpy_s( char * dest, size_t dest_size, const char *src, size_t count ) {
    char * ptr;
    size_t available, remaining;

    // Initialize locals
    ptr = dest;
    available = dest_size;
    remaining = count;

    // Copy src to dest until src is '\0' or dest is full or count reached
    while ((*ptr++ = *src++) != 0 && --available > 0 && --remaining > 1) {
    }

    // Check if dest was too small
    if (available == 0) {
        std::cerr << "SCL internal error (scl_strcpy_s): dest buffer too small." << std::endl;
        exit(1);
    }

    // Terminate dest with '\0'
    *ptr++ = '\0';

    return dest;
}

/**
    Function: scl_strcat_s
    Description:
        Wrapper for strcat function, checks for overflow on dest buffer.
*/
char * scl_strcat_s( char * dest, size_t dest_size, const char *src ) {
    char * ptr;
    size_t  available;

    // Initialize locals
    ptr = dest;
    available = dest_size;

    // Forward to end of dest '\0'
    while (available > 0 && *ptr != 0) {
        ptr++;
        available--;
    }

    // Check if end of dest is found
    if (available == 0) {
        // TODO: throw exception in stead of exit(1)?
        std::cerr << "SCL internal error (scl_strcat_s): dest buffer is not terminated by '\0'." << std::endl;
        exit(1);
    }

    // Copy src to dest until end of src '\0' or dest is full
    while ((*ptr++ = *src++) != 0 && --available > 0) {
    }

    // Check if dest was too small
    if (available == 0) {
        // TODO: throw exception in stead of exit(1)?
        std::cerr << "SCL internal error (scl_strcat_s): dest buffer too small." << std::endl;
        exit(1);
    }

    // Terminate dest with '\0'
    *ptr++ = '\0';

    return dest;
}

/**
    Function: scl_strncat_s
    Description:
        Wrapper for strncat function, checks for overflow on dest buffer.
*/
char * scl_strncat_s( char *dest, size_t dest_size, const char *src, size_t count ) {
    char *ptr;
    size_t available;
    size_t remaining;
    
    // Initialize locals
    ptr = dest;
    available = dest_size;
    remaining = count;

    // Forward to end of dest '\0'
    while (available > 0 && remaining > 1 && *ptr != 0) {
        ptr++;
        available--;
        remaining--;
    }

    // Check if end of dest is found
    if (available == 0) {
        // TODO: throw exception in stead of exit(1)?
        std::cerr << "SCL internal error (scl_strncat_s): dest buffer is not terminated by '\0'." << std::endl;
        exit(1);
    }

    // Copy src to dest until end of src '\0' or dest is full or count reached
    while ((*ptr++ = *src++) != 0 && --available > 0 && --remaining > 1) {
    }

    // Check if dest was too small
    if (available == 0) {
        // TODO: throw exception in stead of exit(1)?
        std::cerr << "SCL internal error (scl_strncat_s): dest buffer too small." << std::endl;
        exit(1);
    }

    // Terminate dest with '\0'
    *ptr = '\0';

    return dest;
}

/**
    Function: scl_sprintf_s
    Description:
        Wrapper for sprintf function, check for overflow on dest buffer.
*/
int scl_sprintf_s( char *dest, size_t dest_size, const char * format, ... ) {
    va_list args;
    va_start( args, format );
    return scl_vsprintf_s(dest, dest_size, format, args);
}

/**
    Function: scl_vsprintf_s
    Description:
        Wrapper for vsprintf function, check for overflow on dest buffer.
*/
int scl_vsprintf_s( char *dest, size_t dest_size, const char * format, va_list args ) {
    int count;
    
    count = vsnprintf(dest, dest_size, format, args);

    if (count == dest_size) {
        // TODO: throw exception in stead of exit(1)?
        std::cerr << "SCL internal error (scl_scl_vsprintf_s): dest buffer too small." << std::endl;
        exit(1);
    }

    return count;
}
