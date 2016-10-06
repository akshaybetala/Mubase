a.exe:	testsp.o spimpl.o sputil.o
	g++ testsp.o spimpl.o sputil.o 
testsp.o:	testsp.cpp sp.h
	g++ -c testsp.cpp
spimpl.o:	spimpl.cpp sp.h sputil.h
	g++ -c spimpl.cpp
sputil.o:	sputil.cpp sputil.h
	g++ -c sputil.cpp
