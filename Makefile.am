bin_PROGRAMS = gif-author

AM_CXXFLAGS = $(GTKMM_CFLAGS) $(CV2_CFLAGS) $(FFMPEG_CFLAGS) -D__STDC_CONSTANT_MACROS

gif_author_SOURCES =  src/main.cc  \
					  src/mainwindow.cc  src/mainwindow.h \
					  src/colorquantizer.cc  src/colorquantizer.h \
					  src/gifauthor.cc  src/gifauthor.h \
					  src/gifencoder.cc  src/gifencoder.h \
					  src/widgets/imagearea.cc src/widgets/imagearea.h \
					  src/input/videoframe.cc src/input/videoframe.h \
					  src/input/videofile.cc src/input/videofile.h \
					  src/output/LZW.cc src/output/LZW.h \
					  src/output/gif.cc src/output/gif.h \
					  src/navigation.cc src/navigation.h \
					  src/pages.cc src/pages.h \
					  src/widgets/videocontrol.cc src/widgets/videocontrol.h \
					  src/widgets/videoplayer.cc src/widgets/videoplayer.h \
					  src/widgets/videoclipper.cc src/widgets/videoclipper.h \
					  src/widgets/videoscrollbar.cc src/widgets/videoscrollbar.h 
gif_author_LDADD = $(GTKMM_LIBS) $(CV2_LIBS) $(FFMPEG_LIBS)
