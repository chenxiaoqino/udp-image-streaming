all: server client

server: Server.cpp PracticalSocket.cpp
	g++ Server.cpp PracticalSocket.cpp -o server

client: Client.cpp PracticalSocket.cpp
	g++ Client.cpp PracticalSocket.cpp -o client


clean:
	rm *.o client server
