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

/* Perform the "process-watcher capture" command.  */
void
capture (void);

/* Perform the "process-watcher get" command.  */
void
get (char *pid_string, char *begin_string, char *end_string);
