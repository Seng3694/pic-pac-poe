#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef parse_result (*cli_func)(const char *arg, args *const a);

static parse_result print_help(const char *arg, args *const a) {
  (void)arg;
  (void)a;

  puts("Usage: ppp [OPTION]\n"
       "Reads png image paths from stdin, loads and packs them into a single "
       "texture and exports metadata to a file.\n"
       "Example: find . -name '*.png' | ppp\n\n"
       "Options:\n"
       "  -h,  --help            prints this help text\n"
       "  -ps, --packer-script   sets the lua script for packing textures\n"
       "  -po, --packer-output   sets the packed output file name\n"
       "  -es, --export-script   sets the lua script for exporting metadata\n"
       "  -eo, --export-output   sets the exported output file name\n"
       "  -w,  --width           sets a width constrain for the packed "
       "texture\n");

  return PARSE_RESULT_EXIT;
}

static inline bool validate_arg(const char *arg) {
  return !(arg == NULL || arg[0] == '\0' || arg[0] == '-');
}

static parse_result set_string(const char *arg, const char **const dest) {
  if (!validate_arg(arg))
    return PARSE_RESULT_FAIL;
  *dest = arg;
  return PARSE_RESULT_SUCCESS;
}

static inline parse_result set_packer_script(const char *arg, args *const a) {
  return set_string(arg, &a->packerScript);
}

static inline parse_result set_packer_output(const char *arg, args *const a) {
  return set_string(arg, &a->packerOutputFile);
}

static inline parse_result set_export_script(const char *arg, args *const a) {
  return set_string(arg, &a->exporterScript);
}

static inline parse_result set_export_output(const char *arg, args *const a) {
  return set_string(arg, &a->exporterOutputFile);
}

static parse_result set_number(const char *arg, int *number) {
  if (!validate_arg(arg))
    return false;
  char *end = NULL;
  int num = strtol(arg, &end, 10);
  if (end == NULL || end == arg || num < 0)
    return false;
  *number = num;
  return PARSE_RESULT_SUCCESS;
}

static inline parse_result set_width_constraint(const char *arg,
                                                args *const a) {
  return set_number(arg, &a->widthConstraint);
}

typedef struct {
  const char shortLabel[4];
  const char label[32];
  const char error[256];
  cli_func parse;
  bool required;
} cli_arg;

const static cli_arg cli_args[] = {
    {"-h", "--help", "", print_help, false},
    {"-ps", "--packer-script", "invalid packer script", set_packer_script,
     true},
    {"-po", "--packer-output", "invalid packer output", set_packer_output,
     true},
    {"-es", "--export-script", "invalid export script", set_export_script,
     true},
    {"-eo", "--export-output", "invalid export output", set_export_output,
     true},
    {"-w", "--width", "invalid width constraint", set_width_constraint, false},
};

const static int cli_args_count = sizeof(cli_args) / sizeof(cli_arg);

parse_result parse_args(const int argc, const char *const *const argv,
                        args *const a) {
  bool argsSet[cli_args_count];
  for (int i = 0; i < cli_args_count; ++i)
    argsSet[i] = false;

  for (int i = 1; i < argc; i += 2) {
    for (int j = 0; j < cli_args_count; ++j) {
      if (strcmp(argv[i], cli_args[j].shortLabel) == 0 ||
          strcmp(argv[i], cli_args[j].label) == 0) {
        const char *nextArg = i + 1 < argc ? argv[i + 1] : NULL;
        const parse_result result = cli_args[j].parse(nextArg, a);
        if (result == PARSE_RESULT_FAIL) {
          fprintf(stderr, "error parsing argument '%s': %s.\n", nextArg,
                  cli_args[j].error);
          return PARSE_RESULT_FAIL;
        } else if (result == PARSE_RESULT_EXIT) {
          return PARSE_RESULT_EXIT;
        }
        argsSet[j] = true;
        break;
      }
    }
  }

  bool hasMissingArg = false;
  for (int i = 0; i < cli_args_count; ++i) {
    if (!argsSet[i] && cli_args[i].required) {
      fprintf(stderr, "missing option: %s\n", cli_args[i].label);
      hasMissingArg = true;
    }
  }

  return hasMissingArg ? PARSE_RESULT_FAIL : PARSE_RESULT_SUCCESS;
}
