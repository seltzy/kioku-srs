#ifndef _KIOKU_ERROR_H
#define _KIOKU_ERROR_H

#include "kioku/result.h"

typedef struct _srsERROR_DATA_s
{
  /* Basic information */
  srsRESULT   code;             /* The result code associated with the error */
  const char *name;             /* The error name */
  const char *message;          /* A more detailed error description */
  /* The following have to do with where the error originated from */
  const char *file_name;        /* Origin file */
  const char *function_name;    /* Origin function */
  int32_t     line_number;      /* Origin line */
  /* Extras */
  int32_t     log_line_number;  /* The line in the log file that was created when this error occurred. -1 if no associated log was printed. */
  void       *dynamic_userdata; /* Dynamically allocated error-specific userdata. Depends on the source. Generally left alone. */
} srsERROR_DATA;

kiokuAPI srsERROR_DATA srsError_Get();

kiokuAPI void srsError_Reset();

kiokuAPI void srsError_Set(srsRESULT code, const char *name, const char *message,
                                    const char *_FILENAME, int32_t line_number, const char _FUNCNAME);

kiokuAPI void srsError_SetExtra(int32_t log_line_number, void *userdata);

#define srsERROR_SET(code, name_getter, msg)          \
  do {                                                \
    srsError_Reset();                                 \
    const char *error_code_name = name_getter(code);  \
    srsError_Set(code, error_code_name, message,      \
                 __FILE__, __LINE__, __FUNC__);       \
  } while(0)

/* This will take the last set error, create a log for it, then retroactively set its log line number. */
#define srsERROR_LOG()                                                  \
  do {                                                                  \
    srsERROR_DATA data = srsError_Get();                                \
    srsLOG_ERROR("%s:%d[%s] | Error #%d (%s): %s",                      \
                 data.file_name, data.line_number, data.function_name,  \
                 data.code, data.name, data.message);                   \
    /** TODO make the srsLOG_* functions more robust and wrappable, and have them return an integer representing the logged line number */ \
  } while(0)

#endif /* _KIOKU_ERROR_H */