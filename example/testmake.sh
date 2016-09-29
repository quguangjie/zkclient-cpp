gcc -o test test.cpp  ../lib/libzkclient.a -I zookeeper-3.4.6/src/c/include/   -I appzkclient/ -lboost_thread-mt -lboost_date_time -lthrift  -l zookeeper_mt -lpthread 
