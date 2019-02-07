FTP:
	g++ -o ./bin/myclient -g ./src/FTP_Client.cpp ./src/simpleSocket.cpp -lpthread
	g++ -o ./bin/myserver ./src/FTP_Server.cpp ./src/simpleSocket.cpp
clean:
	-rm -f ./bin/*
