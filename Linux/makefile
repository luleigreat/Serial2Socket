test:comdev.o zini.o config.o main.o 
	g++ -o test main.o comdev.o zini.o config.o -lpthread
comdev.o:comdev.c comdev.h
	g++ -c comdev.c
zini.o:zini.cpp zini.h
	g++ -c zini.cpp
config.o:config.cpp config.h zini.h
	g++ -c config.cpp
main.o:main.c comdev.h config.h
	g++ -c main.c  -lpthread

clean:
	rm -f *.o
