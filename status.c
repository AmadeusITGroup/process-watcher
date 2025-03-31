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

#include "status.h"

#include <string.h>             /* strchr ().  */
#include <stdlib.h>             /* exit ().  */
#include <assert.h>             /* assert ().  */

/*
 * Example fields from /proc/PID/status:
 *
 * VmPeak:	 1083532 kB
 * VmSize:	 1066808 kB
 * VmLck:	       0 kB
 * VmPin:	       0 kB
 * VmHWM:	  201484 kB
 * VmRSS:	  154232 kB
 * RssAnon:	   96492 kB
 * RssFile:	   49804 kB
 * RssShmem:	    7936 kB
 * VmData:	  195368 kB
 * VmStk:	     132 kB
 * VmExe:	    2848 kB
 * VmLib:	   75476 kB
 * VmPTE:	     652 kB
 * VmSwap:	   14336 kB
 *
 * Note that there is a mix of TAB and SPC characters.
 */

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
process_line_starting_with_token (FILE *status_file, char *token, stat_struct_t *stat_struct)
{
  /* Replace the : in TOKEN with a NUL char.  */
  char *colon = strchr (token, ':');
  if (colon == NULL) {
    fprintf (stderr, "could not find the \":\" character in token \"%s\"\n", token);
    return 1;
  }
  *colon = 0;

  if (0) {
  }
#define X(field)                                                        \
  else if (! strcmp (token, #field)) {                                  \
    int scan = fscanf (status_file, " %d", &stat_struct->field);        \
    if (scan != 1) {                                                    \
      fprintf (stderr, "could not read a number\n");                    \
      return 1;                                                         \
    }                                                                   \
  }
  X(Pid)
  X(PPid)
#include "fields.out.h"
#undef X

  /* Skip until after the next newline.  */
  while (1) {
    int c = fgetc_unlocked (status_file);
    if (c == EOF || c == '\n') {
      return 0;
    }
  }
}

/* Read an open stream on file /proc/PID/status and fill the given
   STAT_STRUCT.  */
int
read_status_file (FILE *status_file, stat_struct_t *stat_struct)
{
  while (1) {
    /* First token of the current line.  Certain tokens can be quite
     * long, e.g. "nonvoluntary_ctxt_switches".  */
    char first_token[32];
    int i = fscanf (status_file, "%31s", first_token);
    if (i == EOF)
      break;

    if (ferror_unlocked (status_file)) {
      perror ("error while reading status file");
      return 1;
    }

    assert (i == 1);

    if (process_line_starting_with_token (status_file, first_token, stat_struct)) {
      fprintf (stderr, "error while processing line starting with token %s\n", first_token);
      return 1;
    }
  }

  return 0;
}

/* Get the ppid and memory information from the given PID and fill the
   given STAT_STRUCT.  */
int
read_status_pid (pid_t pid, stat_struct_t *stat_struct)
{
  /* Initialize everything in stat_struct to 0.  */
  memset (stat_struct, 0, sizeof *stat_struct);

  /* A PID is normally 4 bytes (tested on my x86-64 laptop).
   * Therefore it is 10 digits maximum.
   * So /proc/.../status is 13+10+1 bytes maximum.
   * Let's allocate 32 bytes.  */
  int filename_size = 32;
  char filename[filename_size];
  if (snprintf (filename, filename_size - 1, "/proc/%d/status", pid) >= filename_size - 1) {
    fprintf (stderr, "could not fit /proc/%d/status into filename buffer\n", pid);
    exit (1);
  }
  filename[filename_size - 1] = 0;

  FILE *status_file = fopen (filename, "r");
  if (status_file == NULL) {
    /* The process has just disappeared: ignore it, say it is now
       consuming zero.  */
    return 0;
  }

  /* Increase the buffer size to 4096 instead of the default.  On my
     system, the default is 1024, which always generates 2 reads per
     file instead of 1.  */
  static char buffer[4096];
  /* Note: if we give NULL instead of "buffer", setvbuf will not
     change the buffer size.  This behaviour seems surprising to me,
     given the documentation of this function.  Reading the libc code,
     it seems that the buffer allocation takes place only at the first
     need, not at fopen time.  Reading libio/filedoalloc.c, it seems
     that there is a stat() call during allocation that checks the
     st_blksize and use that or BUFSIZ=8192, whichever is lower.  On
     /proc/self/status, blksize is 1024...  */
  if (setvbuf (status_file, buffer, _IOFBF, 4096)) {
    fprintf (stderr, "could not set status buffer size to 4096\n");
    return 1;
  }

  if (read_status_file(status_file, stat_struct)) {
    fprintf (stderr, "could not read status file %s\n", filename);
    return 1;
  }

  if (fclose (status_file)) {
    fprintf (stderr, "fclose failed: ");
    perror ("");
    return 1;
  }

  return 0;
}
