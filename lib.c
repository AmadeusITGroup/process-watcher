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

/* Enable GNU extensions such as fputs_unlocked ().  */
#define _GNU_SOURCE

#include "get-all-pids.h"       /* get_all_pids ().  */
#include "status.h"             /* read_status_pid ().  */
#include "locks.h"              /* write_lock ().  */
#include "parse-time.h"         /* parse_time ().  */

#include <sys/mman.h>           /* mmap ().  */
#include <stdio.h>              /* FILE.  */
#include <stdlib.h>             /* exit ().  */
#include <errno.h>              /* errno.  */
#include <unistd.h>             /* sleep ().  */
#include <assert.h>             /* assert ().  */
#include <string.h>             /* strerror ().  */
#include <time.h>               /* time ().  */
#include <limits.h>             /* INT_MAX.  */
#include <fcntl.h>              /* open ().  */
#include <sys/stat.h>           /* fstat ().  */

/*
 * FILE FORMAT
 *
 * General remarks:
 * - Field names are case-sensitive.
 * - Numbers are in host order.
 * - pid and memory field values are ints.
 *
 * File header: "# process-watcher file format\n" (without NUL character).
 * Then a sequence of snapshots.  Each snapshot looks like this:
 * - time_t timestamp
 * - Number of processes in the snapshot, as int
 * - sequence of process info, in ascending PID number.  Each process
 *   info is a struct stat_struct_t which looks like this:
 *   - int pid
 *   - int ppid
 *   - int VmPeak
 *   - ...
 */

/* First bytes of any history file.
   The strlen is a multiple of 4 for alignment purposes.  */
static const char header[] = "# process-watcher file format\n\n\n";

/* History file name.  */
static const char capture_filename[] = "process-watcher.out";

/* Perform the "process-watcher capture" command.  */
void
capture (void)
{
  FILE *output = fopen (capture_filename, "w");
  if (output == NULL) {
    perror ("could not open process-watcher.out for writing");
    exit (1);
  }

  int fd = fileno_unlocked (output);
  if (fd == -1) {
    fprintf (stderr, "could not get file descriptor for %s\n", capture_filename);
    exit (1);
  }

  fputs_unlocked (header, output);
  if (ferror_unlocked (output)) {
    fprintf (stderr, "write error while writing the header: %s\n", strerror (errno));
    exit (1);
  }

  /* Loop, one iteration per sample.  */
  while (1) {
    /* Take a write lock before writing to the file.  */
    if (write_lock (fd)) {
      fprintf (stderr, "could not take a write lock on file %s\n", capture_filename);
      exit (1);
    }

    time_t now = time (NULL);
    if (fwrite_unlocked (&now, 1, sizeof now, output) != sizeof now) {
      fprintf (stderr, "write error while writing a timestamp\n");
      exit (1);
    }

    pid_t *pids;
    int nbpids;
    get_all_pids (&pids, &nbpids);

    /* Write the number of pids in this capture.  */
    if (fwrite_unlocked (&nbpids, 1, sizeof nbpids, output) != sizeof nbpids) {
      fprintf (stderr, "could not write to the record: %s\n", strerror (errno));
      exit (1);
    }

    /* Loop, iterate over all processes.  */
    for (int i = 0; i < nbpids; i++) {
      pid_t pid = pids[i];
      stat_struct_t stat_struct;

      if (read_status_pid (pid, &stat_struct)) {
        fprintf (stderr, "could not read status from pid %d\n", pid);
        exit (1);
      }

      if (fwrite_unlocked (&stat_struct, 1, sizeof stat_struct, output) != sizeof stat_struct) {
        fprintf (stderr, "could not write a capture: ");
        perror ("");
        exit (1);
      }
    }

    fflush_unlocked (output);

    /* Release the lock.  */
    if (unlock (fd)) {
      fprintf (stderr, "could not unlock file %s\n", capture_filename);
      exit (1);
    }

    sleep (2);
  }
}

/* Comparator for pointers to stat_struct_t.
   Compare according to stat_struct->Pid.
   Returns <, ==, >0 if a is <, ==, >b.  */
static int
compare_stat_structs (const void *a, const void *b)
{
  const stat_struct_t *da = (const stat_struct_t *) a;
  const stat_struct_t *db = (const stat_struct_t *) b;
  return (da->Pid > db->Pid) - (da->Pid < db->Pid);
}

/* Given the snapshot defined by (SNAPSHOT_START, SNAPSHOT_SIZE),
   determine whether the process identified by CANDIDATE_PROC
   is a subprocess of TOP_PROC (recursively).  */
static int
is_proc_descendant_of_proc (stat_struct_t *candidate_proc, stat_struct_t *top_proc, stat_struct_t *snapshot_start, size_t snapshot_size)
{
  while (1) {
    if (candidate_proc == top_proc) {
      return 1;
    }

    /* Find the parent of candidate.  */
    stat_struct_t dummy_parent = {
      .Pid = candidate_proc->PPid,
    };
    stat_struct_t *parent_proc = (stat_struct_t *) bsearch (&dummy_parent, snapshot_start, snapshot_size, sizeof (stat_struct_t), compare_stat_structs);
    if (parent_proc == NULL) {
      /* Could not find the parent. */
      return 0;
    }

    if (candidate_proc == parent_proc) {
      /* We have detected a tight loop, candidate_proc is its own
         parent.  */
      return 0;
    }

    /* Now we need to know if parent_proc is a descendant of top_proc.  */
    candidate_proc = parent_proc;
  }
}

/* Perform the "process-watcher get" command.  */
void
get (char *pid_string, char *begin_string, char *end_string)
{
  errno = 0;
  long parsed_long = strtol (pid_string, NULL, 10);
  if (errno) {
    fprintf (stderr, "could not parse pid string %s: ", pid_string);
    perror ("");
    exit (1);
  }
  if (parsed_long < 1 || parsed_long > INT_MAX) {
    fprintf (stderr, "cannot decode pid %s: out of range\n", pid_string);
    exit (1);
  }
  int top_pid = (int) parsed_long;

  time_t begin = parse_time (begin_string);
  time_t end = parse_time (end_string);

  if (begin > end) {
    fprintf (stderr, "bad time range: the beginning is after the end\n");
    exit (1);
  }

  int fd = open (capture_filename, O_RDONLY, 0);
  if (fd < 0) {
    perror ("could not open process-watcher.out for reading");
    exit (1);
  }

  struct stat st;
  if (fstat (fd, &st)) {
    perror ("could not stat process-watcher.out");
    exit (1);
  }

  size_t map_len = st.st_size;

  if (read_lock (fd)) {
    fprintf (stderr, "could not take a read lock\n");
    exit (1);
  }

  char *map = (char *) mmap (NULL, map_len, PROT_READ, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    perror ("could not mmap process-watcher.out");
    exit (1);
  }

  char *cursor = map;
  char *map_end = map + map_len;

  const size_t header_len = strlen (header);
  if (cursor + header_len > map_end) {
    fprintf (stderr, "bad file: too short to contain the header\n");
    exit (1);
  }
  if (memcmp (header, cursor, header_len) != 0) {
    fprintf (stderr, "bad header: should be {%s}\n", header);
    exit (1);
  }
  cursor += header_len;

  stat_struct_t max;
  memset (&max, 0, sizeof max); /* Set each element to 0.  */

  /* Loop, one iteration per sample.  */
  while (1) {
    if (cursor == map_end) {
      /* We have reached the end of the file, stop here.  */
      break;
    }

    if (cursor + sizeof (time_t) > map_end) {
      fprintf (stderr, "cursor=%p map_end=%p no room for the timestamp\n", cursor, map_end);
      exit (1);
    }
    time_t timestamp = * (time_t *) cursor;
    cursor += sizeof (time_t);

    if (cursor + sizeof (int) > map_end) {
      fprintf (stderr, "truncated snapshot: no nbpids: cursor=%p map_end=%p\n", cursor, map_end);
      exit (1);
    }
    int nbpids = * (int *) cursor;
    cursor += sizeof (int);

    stat_struct_t *snapshot_start = (stat_struct_t *) cursor;
    size_t snapshot_count = nbpids;
    stat_struct_t *snapshot_end = snapshot_start + snapshot_count;
    if (snapshot_end > (stat_struct_t *) map_end) {
      fprintf (stderr, "truncated snapshot: %d pids,\n  %p  snapshot_start\n  %p  snapshot_end,\n  %p  map_end\n", nbpids, snapshot_start, snapshot_end, map_end);
      abort ();
      exit (1);
    }

    /* Move cursor past the snapshot.  */
    cursor = (char *) snapshot_end;

    if (timestamp < begin) {
      /* Current snapshot is before the requested time window.  */
      continue;
    }

    if (timestamp > end) {
      /* Current snapshot is after of the requested time window.  */
      break;
    }

    /* Find the top process.  */
    stat_struct_t dummy_top = {
      .Pid = top_pid,
    };
    stat_struct_t *top_proc = (stat_struct_t *) bsearch (&dummy_top, snapshot_start, snapshot_count, sizeof (stat_struct_t), compare_stat_structs);
    if (top_proc == NULL) {
      /* Cannot find the requested process.  */
      continue;
    }

    stat_struct_t snapshot_totals;
    memset (&snapshot_totals, 0, sizeof snapshot_totals); /* Set each element to 0.  */

    /* Loop, iterate over all processes.  */
    for (int procidx = 0; procidx < nbpids; procidx++) {
      stat_struct_t *candidate_proc = snapshot_start + procidx;
      if (is_proc_descendant_of_proc (candidate_proc, top_proc, snapshot_start, snapshot_count)) {
        /* Add the current process to the accumulator.  */
#define X(field) snapshot_totals.field += candidate_proc->field;
#include "fields.out.h"
#undef X
      }
    }

    /* Update max according to snapshot_totals.  */
#define X(field)                                \
    if (snapshot_totals.field > max.field) {    \
      max.field = snapshot_totals.field;        \
    }
#include "fields.out.h"
#undef X
  }

  printf ("Max values:\n");
#define X(field) printf (" %20d  %s\n", max.field, #field);
#include "fields.out.h"
#undef X

  munmap (map, map_len);

  unlock (fd);

  close (fd);
}
