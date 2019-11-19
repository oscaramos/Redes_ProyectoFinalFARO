FLAGS = -std=c++11 -pthread -Wall
AUX_HEADERS = stringhelper.h Safe.h SimulatorUserInput.h
PEER_HEADERS = PeerConnection.h SenderSocket.h ReceiverSocket.h
CLIENT_HEADERS = ${PEER_HEADERS} Client.h
SERVER_HEADERS = ${PEER_HEADERS} Server.h

AUX_HEADERS := $(addprefix aux/, $(AUX_HEADERS))
CLIENT_HEADERS := $(addprefix peer/, $(CLIENT_HEADERS))
SERVER_HEADERS := $(addprefix peer/, $(SERVER_HEADERS))

# MY_HEADERS = GamePuntito.h


all: client.exe server.exe
	@echo Terminado!!

server.exe: server.cpp ${AUX_HEADERS} ${SERVER_HEADERS}
	g++ server.cpp -o server.exe ${FLAGS}


client.exe: client.cpp ${AUX_HEADERS} ${CLIENT_HEADERS}
	g++ client.cpp -o client.exe ${FLAGS}

clear:
	rm ./client.exe ./server.exe
