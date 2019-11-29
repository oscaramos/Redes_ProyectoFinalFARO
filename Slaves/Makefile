FLAGS = -std=c++11 -pthread -Wall
AUX_HEADERS = stringhelper.h Safe.h SimulatorUserInput.h
PEER_HEADERS = PeerConnection.h SenderSocket.h ReceiverSocket.h
CLIENT_HEADERS = ${PEER_HEADERS} Client.h
SERVER_HEADERS = ${PEER_HEADERS} Server.h

AUX_HEADERS := $(addprefix aux/, $(AUX_HEADERS))
CLIENT_HEADERS := $(addprefix peer/, $(CLIENT_HEADERS))
SERVER_HEADERS := $(addprefix peer/, $(SERVER_HEADERS))
MY_HEADERS = auxPackCmd.h Database.h MasterConnection.h SlaveClientConnection.h SlaveServerConnection.h globals.h

all: slave.exe
	@echo Terminado!!

slave.exe: slave.cpp ${AUX_HEADERS} ${CLIENT_HEADERS} ${SERVER_HEADERS} ${MY_HEADERS}
	g++ slave.cpp -o slave.exe ${FLAGS}

debug: slave.cpp ${AUX_HEADERS} ${CLIENT_HEADERS} ${SERVER_HEADERS} ${MY_HEADERS}
	g++ slave.cpp -o slave.exe ${FLAGS} -g

clear:
	rm ./slave.exe
