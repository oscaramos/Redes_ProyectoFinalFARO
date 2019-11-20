FLAGS = -std=c++11 -pthread -Wall
AUX_HEADERS = stringhelper.h Safe.h SimulatorUserInput.h
PEER_HEADERS = PeerConnection.h SenderSocket.h ReceiverSocket.h
CLIENT_HEADERS = ${PEER_HEADERS} Client.h
SERVER_HEADERS = ${PEER_HEADERS} Server.h

AUX_HEADERS := $(addprefix aux/, $(AUX_HEADERS))
CLIENT_HEADERS := $(addprefix peer/, $(CLIENT_HEADERS))
SERVER_HEADERS := $(addprefix peer/, $(SERVER_HEADERS))


all: client.exe
	@echo Terminado!!

client.exe: client.cpp ${AUX_HEADERS} ${CLIENT_HEADERS}
	g++ client.cpp -o client.exe ${FLAGS}

clear:
	rm ./client.exe
