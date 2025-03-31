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

#include <stdio.h>              /* FILE.  */
#include <unistd.h>             /* pid_t.  */

#include "stat-struct.h"        /* stat_struct_t.  */

/* Process one line of /proc/PID/status, storing the value into STAT_STRUCT.

   STATUS_FILE is an open stream on /proc/PID/status.

   The caller shall read from STATUS_FILE the first token of a line
   into TOKEN, then call
   process_line_starting_with_token(). process_line_starting_with_token
   reads the rest of the line, extracts the value and stores it into
   STAT_STRUCT if it corresponds to one of the watched tokens.

   TOKEN should be the first token of a line, including any ":",
   e.g. "VmRSS:".
*/
int
process_line_starting_with_token (FILE *status_file, char *token, stat_struct_t *stat_struct);

/* Read an open stream on file /proc/PID/status and fill the given
   STAT_STRUCT.  */
int
read_status_file (FILE *status_file, stat_struct_t *stat_struct);

/* Get the ppid and memory information from the given PID and fill the
   given STAT_STRUCT.  */
int
read_status_pid (pid_t pid, stat_struct_t *stat_struct);
