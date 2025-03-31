bin_PROGRAMS = process-watcher
process_watcher_SOURCES = \
  process-watcher.c \
  get-all-pids.c \
  lib.c \
  locks.c \
  parse-time.c \
  status.c \
  string-has-only-digits.c \
  xmalloc.c

TESTS = unittests

CLEANFILES = fields.out.h

unittests: \
  get-all-pids.o \
  status.test.o \
  status.o \
  string-has-only-digits.o \
  string-has-only-digits.test.o \
  xmalloc.o

lib.o: fields.out.h
status.o: fields.out.h

fields.out.h: fields
	$(AWK) '/#/ {next} /./ {print "X(" $$0 ")"}' < $< > $@
