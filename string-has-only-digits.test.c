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

#include "string-has-only-digits.h"

#include <stdio.h>              /* fprintf ().  */
#include <stdlib.h>             /* exit ().  */

/* Run one test on the string_has_only_digits function.  */
static int
test_string_has_only_digits_args (const char *str, int expected) {
  int error = 0;

  int actual;
  actual = string_has_only_digits (str);
  if (actual != expected) {
    fprintf (stderr, "test_string_has_only_digits_args: \"%s\" should give %d but gave %d\n", str, expected, actual);
    error = 1;
  }

  return error;
}

/* Run all tests on the string-has-only-digits.c file.  */
void
test_string_has_only_digits (void)
{
  int error = 0;

  error += test_string_has_only_digits_args ("012345", 1);
  error += test_string_has_only_digits_args ("01234S", 0);

  if (error) {
    exit (1);
  }
}

