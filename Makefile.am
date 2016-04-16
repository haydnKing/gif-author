bin_PROGRAMS = gif-author

AM_CXXFLAGS = $(GTKMM_CFLAGS) $(CV2_CFLAGS) $(FFMPEG_CFLAGS) -D__STDC_CONSTANT_MACROS -std=c++11

gif_author_SOURCES = src/segmenter.cc \
					 src/bitset.cc \
					 src/navigation.cc \
					 src/gifencoder.cc \
					 src/gifauthor.cc \
					 src/output/LZW.cc \
					 src/output/gif.cc \
					 src/widgets/videoclipper.cc \
					 src/widgets/videocontrol.cc \
					 src/widgets/imagearea.cc \
					 src/widgets/videoplayer.cc \
					 src/widgets/videoscrollbar.cc \
					 src/mainwindow.cc \
					 src/colorquantizer.cc \
					 src/main.cc \
					 src/input/videofile.cc \
					 src/input/videoframe.cc \
					 src/pages.cc

gif_author_LDADD = $(GTKMM_LIBS) $(CV2_LIBS) $(FFMPEG_LIBS)
