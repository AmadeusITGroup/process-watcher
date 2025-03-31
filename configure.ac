#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([process-watcher], [1.0], [fbauzac@amadeus.com])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_SRCDIR([get-all-pids.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_AWK
if ! test -n "$AWK"; then
  AC_MSG_ERROR([awk is required])
fi

AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h])
if ! test "x$ac_cv_header_fcntl_h" = xyes; then
  AC_MSG_ERROR([fcntl.h is required])
fi
if ! test "x$ac_cv_header_unistd_h" = xyes; then
  AC_MSG_ERROR([unistd.h is required])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
if ! test "x$ac_cv_type_pid_t" = xyes; then
  AC_MSG_ERROR([pid_t is required])
fi
AC_TYPE_SIZE_T
if ! test "x$ac_cv_type_size_t" = xyes; then
  AC_MSG_ERROR([size_t is required])
fi

# Checks for library functions.
AC_CHECK_FUNCS([memset strchr strerror strtol mmap munmap])
if ! test "x$ac_cv_func_memset" = xyes; then
  AC_MSG_ERROR([memset is required])
fi
if ! test "x$ac_cv_func_strchr" = xyes; then
  AC_MSG_ERROR([strchr is required])
fi
if ! test "x$ac_cv_func_strerror" = xyes; then
  AC_MSG_ERROR([strerror is required])
fi
if ! test "x$ac_cv_func_strtol" = xyes; then
  AC_MSG_ERROR([strtol is required])
fi
if ! test "x$ac_cv_func_mmap" = xyes; then
  AC_MSG_ERROR([mmap is required])
fi
if ! test "x$ac_cv_func_munmap" = xyes; then
  AC_MSG_ERROR([munmap is required])
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
