bin_PROGRAMS = gif-author

AM_CXXFLAGS = $(CV2_CFLAGS) $(FFMPEG_CFLAGS) -D__STDC_CONSTANT_MACROS -std=c++11

gif_author_SOURCES = src/util/cline.cc \
 					 src/output/gif.cc \
 					 src/output/LZW.cc \
 					 src/encoder/colorquantizer.cc \
 					 src/encoder/ditherer.cc \
 					 src/util/bitset.cc \
 					 src/video/videofile.cc \
 					 src/video/videoframe.cc \
					 src/encoder/segmenter.cc \
 					 src/encoder/gifencoder.cc \
 					 src/gifauthor.cc \
					 src/main.cc 
					 

gif_author_LDADD = $(CV2_LIBS) $(FFMPEG_LIBS)
