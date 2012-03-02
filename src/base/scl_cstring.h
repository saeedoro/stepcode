#ifndef SCL_CSTRING_H
#define SCL_CSTRING_H

#include <scl_export.h>
#include <stdarg.h>

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#endif

SCL_BASE_EXPORT char * scl_strcpy_s( char * dest, size_t dest_size, const char *src );
SCL_BASE_EXPORT char * scl_strncpy_s( char * dest, size_t dest_size, const char *src, size_t count );
SCL_BASE_EXPORT char * scl_strcat_s( char * dest, size_t dest_size, const char *src );
SCL_BASE_EXPORT char * scl_strncat_s( char *dest, size_t dest_size, const char *src, size_t num );
SCL_BASE_EXPORT int    scl_sprintf_s( char *dest, size_t dest_size, const char * format, ... );
SCL_BASE_EXPORT int    scl_vsprintf_s( char *dest, size_t dest_size, const char * format, va_list args );

#ifdef __cplusplus
}
#endif

#endif /* SCL_CSTRING_H */
