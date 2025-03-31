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

#include "parse-time.h"

#include <string.h>             /* strlen ().  */
#include <stdio.h>              /* fprintf ().  */
#include <stdlib.h>             /* exit ().  */
#include <assert.h>             /* assert ().  */

/* Parse the given STRING and convert into time_t.
   STRING is supposed to have the format: YYYYMMDDhhmmss.  */
time_t
parse_time (char *string)
{
  /* String is supposed to be: YYYYMMDDhhmmss i.e. 14 chars.  */
  if (strlen (string) != 14) {
    fprintf (stderr, "time string \"%s\" is not 14 chars long\n", string);
    exit (1);
  }
  struct tm tm;

  int year, month, day, hour, min, sec;
  if (sscanf (string, "%4d%02d%02d%02d%02d%02d", &year, &month, &day, &hour, &min, &sec) != 6) {
    fprintf (stderr, "could not parse time from string %s\n", string);
    exit (1);
  }

  assert (year > 1900);
  tm.tm_year = year - 1900;

  assert (month >= 1);
  assert (month <= 12);
  tm.tm_mon = month - 1;

  assert (day >= 1);
  assert (day <= 31);
  tm.tm_mday = day;

  assert (hour >= 0);
  assert (hour < 24);
  tm.tm_hour = hour;

  assert (min >= 0);
  assert (min < 60);
  tm.tm_min = min;

  assert (sec >= 0);
  assert (sec <= 60);
  tm.tm_sec = sec;

  return timegm (&tm);
}
