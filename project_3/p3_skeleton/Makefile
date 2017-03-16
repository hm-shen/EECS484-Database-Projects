# enables c++11 on CAEN - TODO: check if this is still needed
CXXFLAGS = -g3 -std=c++11

main: bnode_inner.o bnode_leaf.o bnode.o btree.o impl.o mainfile.o
	g++ $(CXXFLAGS) $^ -o main

mainfile.o: mainfile.cpp constants.h
	g++ -c $(CXXFLAGS) mainfile.cpp

bnode_inner.o: bnode_inner.h bnode_inner.cpp
	g++ -c $(CXXFLAGS) bnode_inner.cpp

bnode_leaf.o: bnode_leaf.h bnode_leaf.cpp
	g++ -c $(CXXFLAGS) bnode_leaf.cpp

bnode.o: bnode.h bnode.cpp constants.h
	g++ -c $(CXXFLAGS) bnode.cpp

btree.o: btree.h btree.cpp constants.h
	g++ -c $(CXXFLAGS) btree.cpp

impl.o: impl.cpp constants.h
	g++ -c $(CXXFLAGS) $^

debug: impl.cpp mainfile.cpp btree.cpp bnode_leaf.cpp bnode_inner.cpp bnode.cpp constants.h
	g++ $(CXXFLAGS) mainfile.cpp btree.cpp bnode_leaf.cpp bnode_inner.cpp bnode.cpp impl.cpp -o main

clean:
	rm -f *.o *.gch main a.out

