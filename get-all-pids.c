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

#include "get-all-pids.h"
#include "string-has-only-digits.h"

#include "xmalloc.h"

#include <dirent.h>             /* opendir ().  */
#include <stdio.h>              /* fprintf ().  */
#include <string.h>             /* strerror ().  */
#include <errno.h>              /* errno.  */
#include <stdlib.h>             /* exit ().  */

/* Comparator function for pointers to pid_t.
   Returns <, ==, >0 if a is <, ==, >b.  */
static int
compare_pids (const void *a, const void *b)
{
  const pid_t *da = (const pid_t *) a;
  const pid_t *db = (const pid_t *) b;
  return (*da > *db) - (*da < *db);
}

/* List all PIDs by looking at /proc.
   The PIDs are sorted by ascending PID order.
   Both pids and pnbpids are output arguments.
   The caller should not try deallocating the returned array. */
void
get_all_pids (pid_t ** pids, int *pnbpids) {
  int nbpids = 0;
  static int capacity = 0;
  static pid_t *ret = NULL;

  /* Initialization */
  if (ret == NULL) {
    capacity = 256;
    ret = (pid_t *) xmalloc(capacity * sizeof (pid_t));
  }

  DIR *procdir = opendir("/proc");
  if (procdir == NULL) {
    fprintf (stderr, "could not opendir /proc: %s\n", strerror (errno));
    exit (1);
  }

  while (1) {
    errno = 0;
    struct dirent *dirent = readdir (procdir);
    if (dirent == NULL) {
      if (errno == 0) {
        /* No issue, we just reached the end of the directory.  */
        break;
      } else {
        perror ("could not read a /proc direntry");
        exit (1);
      }
    }

    /* Skip unless the direntry looks like a PID.  */
    if (! string_has_only_digits (dirent->d_name))
      continue;

    /* We have a pid.  */

    /* Make sure we have room to increase nbpids. */
    if (nbpids+1 > capacity) {
      capacity *= 2;
      ret = xreallocarray (ret, capacity, sizeof (pid_t));
    }

    /* Convert dirname to pid_t.  */
    errno = 0;
    long int strtol_value = strtol(dirent->d_name, NULL, 10);
    if (errno) {
      fprintf (stderr, "could not convert {%s} to pid_t: %s\n", dirent->d_name, strerror (errno));
      exit (1);
    }
    pid_t pid = strtol_value;

    /* Add it to the array. */
    ret[nbpids++] = pid;
  }

  if (closedir (procdir)) {
    perror ("could not closedir /proc");
    exit (1);
  }

  qsort (ret, nbpids, sizeof (pid_t), compare_pids);

  *pids = ret;
  *pnbpids = nbpids;
}
