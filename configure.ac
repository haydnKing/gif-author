# Change these to suit your project
AC_INIT([Gif Author], 1, [you@yourproject.com], [gifauthor],
        [http://yourproject.com/])
# If you don't provide HACKING, COPYING, etc, use foreign
AM_INIT_AUTOMAKE([-Wno-portability no-dist-gzip dist-xz foreign subdir-objects])

PKG_CHECK_MODULES([CV2], [opencv >= 3.0.0])
PKG_CHECK_MODULES([FFMPEG], libavutil libavformat libavcodec libswscale)

AC_PROG_CXX

AC_OUTPUT(Makefile)
