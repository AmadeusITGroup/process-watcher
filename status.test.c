/*
This file is part of process-watcher.

process-watcher is free software: you can redistribute it and/or
modify it under the terms of the Apache 2.0 License as published by
the Apache Software Foundation.

process-watcher is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the License
for more details.

You should have received a copy of the Apache 2.0 License
along with process-watcher.
If not, see <https://www.apache.org/licenses/LICENSE-2.0>.
*/

#include "status.h"             /* process_line_starting_with_token ().  */

#include <stdlib.h>             /* exit ().  */
#include <string.h>             /* memset ().  */
#include <assert.h>             /* assert ().  */

/* Define a test on field FIELD.  */
#define DEFINE_test_process_line_starting_with_token_args(field)        \
static int                                                              \
test_process_line_starting_with_token_args_##field (const char *token, const char *file_contents, int expected) { \
  stat_struct_t stat_struct;                                            \
  char token_modifiable[1024];                                          \
  char file_contents_modifiable[1024];                                  \
  FILE *status_file;                                                    \
  int actual;                                                           \
  int error = 0;                                                        \
                                                                        \
  memset (&stat_struct, 0, sizeof stat_struct);                         \
  strcpy (token_modifiable, token);                                     \
  strcpy (file_contents_modifiable, file_contents);                     \
  status_file = fmemopen (file_contents_modifiable, strlen (file_contents_modifiable), "r"); \
  process_line_starting_with_token (status_file, token_modifiable, &stat_struct); \
  actual = stat_struct.field;                                           \
  if (actual != expected) {                                             \
    fprintf (stderr, "error: process_line_starting_with_token(file=\"%s\", token=\"%s\", CAPTURE) resulted in item %s having value %d instead of %d\n", file_contents, token, #field, actual, expected); \
    error = 1;                                                          \
  }                                                                     \
  fclose (status_file);                                                 \
                                                                        \
  return error;                                                         \
}

DEFINE_test_process_line_starting_with_token_args(VmPeak)
DEFINE_test_process_line_starting_with_token_args(VmRSS)

/* Run all tests on the process_line_starting_with_token function.  */
static void
test_process_line_starting_with_token (void) {
  int error = 0;

  error += test_process_line_starting_with_token_args_VmPeak ("VmPeak:", "\t 1037716 kB\nOtherField 54 kB\n", 1037716);
  error += test_process_line_starting_with_token_args_VmRSS ("VmRSS:", "\t  182232 kB\nRssAnon:\t   92008 kB\nRssFile:\t   90224 kB\n", 182232);

  if (error) {
    exit (1);
  }
}

/* Run all tests on the read_status_file function.  */
static void
test_read_status_file (void)
{
  int error = 0;
  static char *test_input =
    ("VmRSS: \t42 kB\n"
     "VmSize: \t24 kB\n"
     "Pid: \t16\n");
  FILE *file = fmemopen (test_input, strlen(test_input), "r");
  assert (file != NULL);
  stat_struct_t stat_struct;
  memset (&stat_struct, 0, sizeof stat_struct);
  int result = read_status_file (file, &stat_struct);
  if (result != 0) {
    fprintf (stderr, "read_status_file returned %d instead of 0\n", result);
    error++;
  }
  if (stat_struct.VmRSS != 42) {
    fprintf (stderr, "VmRSS is %d but should be 42\n", stat_struct.VmRSS);
    error++;
  }
  if (stat_struct.VmSize != 24) {
    fprintf (stderr, "VmSize is %d but should be 24\n", stat_struct.VmSize);
    error++;
  }
  if (stat_struct.Pid != 16) {
    fprintf (stderr, "Pid is %d but should be 16\n", stat_struct.Pid);
    error++;
  }
  if (error)
    exit (1);
  fclose (file);
}

/* Run all tests on the status.c file.   */
void
test_status (void)
{
  test_process_line_starting_with_token ();
  test_read_status_file ();
}
