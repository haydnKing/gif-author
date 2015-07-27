# Change these to suit your project
AC_INIT([Gif Author], 1, [hjking734@gmail.com], [gifauthor],
        [https://github.com/haydnKing/gif-author])
# If you don't provide HACKING, COPYING, etc, use foreign
AM_INIT_AUTOMAKE([-Wno-portability no-dist-gzip dist-xz foreign subdir-objects])
AC_PROG_CC
# Enable C99
AC_PROG_CC_STDC
LT_PREREQ(2.2)
AC_CONFIG_HEADERS([config.h])
AC_PREFIX_DEFAULT(/usr/local)
# Remove if you wish to see full compiler command lines
AM_SILENT_RULES([yes])
LT_INIT([disable-static])

# Request gtk+-30 via pkg-config, >= version 3.10.0
# Access the variables via GTK3_CFLAGS and GTK3_LIBS
PKG_CHECK_MODULES([GTK3], [gtk+-3.0 >= 3.10.0])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
