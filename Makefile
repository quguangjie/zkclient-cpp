include Makefile.in

OBJS = ZkClient.o ZkConnection.o ZkLock.o  ZkWatcher.o  ZkThread.o \
ZkAccept.o ZkRequest.o ZkBase.o ZkLeader.o ZkNodeConn.o \
ZkDistributedLock.o
CFLAGS  += -I inc
LIBPATH += -l zookeeper_mt -lpthread -lm

all: libzkclient.a

libzkclient.a : $(OBJS)
	$(AR) cr $@ $^
	cp libzkclient.a ../lib/

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@ 

%.o:%.cpp
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) $< -c -o $@ 

test:
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) zktest.cpp libzkclient.a -o zktest $(LIBPATH)

test2:
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) zktest2.cpp libzkclient.a -o zktest2 $(LIBPATH)
.PHONY: clean
	
clean:
	rm *.o -rf 
	rm *.a -rf 









