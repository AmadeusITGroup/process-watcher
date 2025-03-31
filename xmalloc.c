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

#include "xmalloc.h"

/* Enable GNU extensions such as fputs_unlocked ().  */
#define _GNU_SOURCE

#include <stdlib.h>             /* malloc ().  */
#include <stdio.h>              /* fputs_unlocked ().  */

void *
xmalloc (size_t size)
{
  void *value = malloc (size);
  if (value == 0) {
    fputs_unlocked ("virtual memory exhausted\n", stderr);
    exit (1);
  }
  return value;
}

void *
xreallocarray (void *ptr, size_t nmemb, size_t size)
{
  void *value = reallocarray (ptr, nmemb, size);
  if (value == 0) {
    fputs_unlocked ("virtual memory exhausted\n", stderr);
    exit (1);
  }
  return value;
}
