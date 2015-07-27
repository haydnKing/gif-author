ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -fstack-protector -Wall -pedantic \
        -Wstrict-prototypes -Wundef -fno-common \
        -Werror-implicit-function-declaration \
        -Wformat -Wformat-security -Werror=format-security \
        -Wconversion -Werror \
        -DDATADIR=\"$(datadir)\"

bin_PROGRAMS = gif-author

gif_author_SOURCES = \
	src/sample-window.c \
	src/sample-window.h \
	src/main.c

gif_author_CFLAGS = \
	$(GTK3_CFLAGS) \
	$(AM_CFLAGS)

gif_author_LDADD = \
	$(GTK3_LIBS)
