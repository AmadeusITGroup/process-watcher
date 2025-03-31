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

#include "locks.h"

#include <fcntl.h>              /* fcntl ().  */
#include <stdio.h>              /* perror ().  */

/* Take a write lock on file descriptor FD.
   On success, return 0; on error, return 1.  */
int
write_lock (int fd)
{
  struct flock l = {
    .l_type = F_WRLCK,
    .l_whence = SEEK_SET,
    .l_start = 0,
    .l_len = 1,
  };
  int status = fcntl (fd, F_SETLKW, &l);
  if (status == -1) {
    perror ("could not writelock file");
    return 1;
  }
  return 0;
}

/* Take a read lock on file descriptor FD.
   On success, return 0; on error, return 1.  */
int
read_lock (int fd)
{
  struct flock l = {
    .l_type = F_RDLCK,
    .l_whence = SEEK_SET,
    .l_start = 0,
    .l_len = 1,
  };
  int status = fcntl (fd, F_SETLKW, &l);
  if (status == -1) {
    perror ("could not readlock file");
    return 1;
  }
  return 0;
}

/* Release the current lock on file descriptor FD.
   On success, return 0; on error, return 1.  */
int
unlock (int fd)
{
  struct flock l = {
    .l_type = F_UNLCK,
    .l_whence = SEEK_SET,
    .l_start = 0,
    .l_len = 1,
  };
  int status = fcntl (fd, F_SETLKW, &l);
  if (status == -1) {
    perror ("could not unlock file");
    return 1;
  }
  return 0;
}
