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

/* Take a write lock on file descriptor FD.
   On success, return 0; on error, return 1.  */
int
write_lock (int fd);

/* Take a read lock on file descriptor FD.
   On success, return 0; on error, return 1.  */
int
read_lock (int fd);

/* Release the current lock on file descriptor FD.
   On success, return 0; on error, return 1.  */
int
unlock (int fd);
