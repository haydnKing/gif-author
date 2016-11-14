bin_PROGRAMS = gif-author

AM_CXXFLAGS = $(GTKMM_CFLAGS) $(CV2_CFLAGS) $(FFMPEG_CFLAGS) -D__STDC_CONSTANT_MACROS -std=c++11

#gif_author_SOURCES = src/encoder/segmenter.cc \
#					 src/encoder/gifencoder.cc \
#					 src/encoder/colorquantizer.cc \
#					 src/encoder/ditherer.cc \
#					 src/util/bitset.cc \
#					 src/util/cline.cc \
#					 src/gifauthor.cc \
#					 src/output/LZW.cc \
#					 src/output/gif.cc \
#					 src/video/videofile.cc \
#					 src/video/videoframe.cc \
#					 src/main.cc 

gif_author_SOURCES = src/util/cline.cc \
					 src/main.cc 
					 

gif_author_LDADD = $(GTKMM_LIBS) $(CV2_LIBS) $(FFMPEG_LIBS)
