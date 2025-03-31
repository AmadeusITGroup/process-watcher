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

#include "lib.h"

#include <stdio.h>		/* printf ().  */
#include <getopt.h>		/* getopt_long ().  */
#include <assert.h>             /* assert ().  */
#include <limits.h>             /* INT_MAX.  */
#include <unistd.h>             /* chdir ().  */
#include <stdlib.h>             /* abort ().  */
#include <string.h>             /* strcmp ().  */

/* Print the help message.  */
static void
usage (void) {
  puts ("process-watcher [OPTION...] capture\n"
        " Start a capturing process.\n"
        "process-watcher [OPTION...] get PID BEGIN END\n"
        " Target the process tree rooted at PID\n"
        " and collect the max of each measure between BEGIN and END times.\n"
        " Times are written as YYYYMMDDhhmmss in UTC.\n"
        "kill PW_PID\n"
        " Stop the capturing process.\n"
        "Options:\n"
        "  -C, --directory=DIR   Use DIR as working directory instead of current working directory.\n"
        "  -h, --help            Show this help.");
}

int
main (int argc, char *argv[]) {
  static const struct option long_opt[] = {
    { "help", no_argument, NULL, 'h' },
    { "directory", required_argument, NULL, 'C' },
    { NULL, 0, NULL, 0 }
  };

  while (1) {
    const int c = getopt_long (argc, argv, "hC:", long_opt, NULL);
    if (c == -1)
      break;
    switch (c) {
    case 'h':
      usage ();
      return 0;
    case 'C':
      if (chdir (optarg)) {
        perror("could not cd to the directory");
        return 1;
      }
      break;
    case '?':
      return 1;
    default:
      abort ();
    }
  }
  argc -= optind; argv += optind;

  if (argc < 1) {
    fprintf (stderr, "missing command\n");
    return 1;
  }

  if (! strcmp (argv[0], "capture")) {
    argc--; argv++;
    if (argc > 0) {
      fprintf (stderr, "too many arguments\n");
      return 1;
    }
    capture ();
    return 0;
  } else if (! strcmp (argv[0], "get")) {
    argc--; argv++;
    /* We expect PID BEGIN END.  */
    if (argc < 3) {
      fprintf (stderr, "missing parameter\n");
      return 1;
    } else if (argc > 3) {
      fprintf (stderr, "too many arguments\n");
      return 1;
    }
    get (argv[0], argv[1], argv[2]);
    return 0;
  } else {
    fprintf (stderr, "invalid command %s\n", argv[0]);
    return 1;
  }

  /* Unreachable */
  assert (0);
  return 0;
}
