CC = g++
LD = g++

CFLAGS = -c -g -O0 --coverage -pedantic -std=c++11 -Wall
LFLAGS = -pedantic -Wall -g -O0 --coverage

OBJS = main.o LogRecord.o LogMgr.o StorageEngine.o
PROG = main

# clean everything first, including logs and databases. Then, build
# the program. Then run the tests.
default: clean $(PROG) tests

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

tests:
	./main testcases/test00
	./main testcases/test01
	./main testcases/test02
	./main testcases/test03
	./main testcases/test04
	./main testcases/test05
	# add additional tests here.
	# finally run the gcov tool to generate the test coverage file.
	gcov LogMgr.cpp
	# finally, view the LogMgr.cpp.gcov file in an editor or using "more".
	# Any lines that start with ##### have not executed in any of your
	# tests. Add more tests for a more thorough testing and manually
	# verify that the output is as you expect.


clean:
	rm -f *.o *.gcno *.gcov *.gcda
	rm -f output/log/*
	rm -f output/dbs/*
