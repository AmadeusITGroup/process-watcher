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

#include <unistd.h>

/* List all PIDs by looking at /proc.
   The PIDs are sorted by ascending PID order.
   Both pids and pnbpids are output arguments.
   The caller should not try deallocating the returned array. */
void
get_all_pids (pid_t ** pids, int *pnbpids);
