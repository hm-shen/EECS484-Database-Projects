CC = g++
LD = g++

CFLAGS = -c -g -pedantic -std=c++11 -Wall
LFLAGS = -pedantic -Wall

OBJS = main.o LogRecord.o LogMgr.o StorageEngine.o
PROG = main

# clean everything first, including logs and databases. Then, build
# the program.
default: clean $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

main.o: StorageEngine/main.cpp StorageEngine/StorageEngine.h StudentComponent/LogMgr.h
	$(CC) $(CFLAGS) StorageEngine/main.cpp

LogRecord.o: StudentComponent/LogRecord.cpp StudentComponent/LogRecord.h
	$(CC) $(CFLAGS) StudentComponent/LogRecord.cpp

LogMgr.o: StudentComponent/LogMgr.cpp StudentComponent/LogMgr.h StorageEngine/StorageEngine.h
	$(CC) $(CFLAGS) StudentComponent/LogMgr.cpp

StorageEngine.o: StorageEngine/StorageEngine.cpp StorageEngine/StorageEngine.h StudentComponent/LogMgr.h
	$(CC) $(CFLAGS) StorageEngine/StorageEngine.cpp

clean:
	rm -f *.o
