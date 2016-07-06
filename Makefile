##############################################################################
# 
#                                               @zeal build in 2016/6/20
##############################################################################

CXX =g++ -ggdb -DLINUX

DEFINES := \
 
LDFLAG= -I./Headers 
#-lpthread
LIBFILE = videoManager
OBJS= Sources/BasicTCP.o \
      Sources/Buffer.o \
      Sources/CityMonitor.o \
      Sources/DateTime.o \
      Sources/Debugger.o \
      Sources/HistoryVideo.o \
      Sources/HistoryVideoManager.o \
      Sources/HistoryVideoPlayer.o \
      Sources/Receiver.o \
      Sources/RemoteCmd.o \
      Sources/Sender.o   \
      Sources/TcpConn.o  \
      Sources/Thread.o   \
      Sources/VideoManager.o  \
      Sources/VideoChannel.o  \
      Sources/VideoRecorder.o \
      Sources/BasicObj.o \
      Sources/ObjBuffer.o

CXX += $(LDFLAG)
$(LIBFILE): $(OBJS)
	          $(CXX) $? -lpthread -o $@ 
	          mv $@  ./CityMonitor_elf 

Debug:$(LIBFILE)	

.PHONY: clean

clean: 
	rm -f $(OBJS) $(LIBFILE)

