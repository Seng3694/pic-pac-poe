#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

typedef struct {
  const char *packerScript;
  const char *packerOutputFile;
  const char *exporterScript;
  const char *exporterOutputFile;
  int widthConstraint;
} args;

typedef enum {
  PARSE_RESULT_SUCCESS,
  PARSE_RESULT_FAIL,
  PARSE_RESULT_EXIT,
} parse_result;

parse_result parse_args(const int argc, const char *const *const argv,
                        args *const a);

#endif
