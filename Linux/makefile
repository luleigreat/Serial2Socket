test:comdev.o zini.o config.o server.o main.o 
	g++ -o test main.o comdev.o zini.o config.o server.o client.o -lpthread
comdev.o:comdev.c comdev.h
	g++ -c comdev.c
zini.o:zini.cpp zini.h
	g++ -c zini.cpp
config.o:config.cpp config.h zini.h
	g++ -std=c++11 -c config.cpp
server.o:CServer.cpp CServer.h config.h util.h comdev.h
	g++ -std=c++11 -c CServer.cpp -o server.o
client.o:CClient.cpp CClient.h config.h util.h comdev.h
	g++ -std=c++11 -c CClient.cpp -o client.o
main.o:main.c comdev.h config.h
	g++ -std=c++11 -c main.c  -lpthread

clean:
	rm -f *.o
