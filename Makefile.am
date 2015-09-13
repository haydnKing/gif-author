bin_PROGRAMS = gif-author

AM_CXXFLAGS = $(GTKMM_CFLAGS) $(CV2_CFLAGS) $(FFMPEG_CFLAGS) -D__STDC_CONSTANT_MACROS

gif_author_SOURCES =  src/main.cc  \
					  src/helloworld.cc  src/helloworld.h \
					  src/imagearea.cc src/imagearea.h \
					  src/videoframe.cc src/videoframe.h \
					  src/videofile.cc src/videofile.h \
					  src/videocontrol.cc src/videocontrol.h \
					  src/videoplayer.cc src/videoplayer.h \
					  src/videoclipper.cc src/videoclipper.h \
					  src/videoscrollbar.cc src/videoscrollbar.h 
gif_author_LDADD = $(GTKMM_LIBS) $(CV2_LIBS) $(FFMPEG_LIBS)
