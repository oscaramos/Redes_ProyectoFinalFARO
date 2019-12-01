FLAGS = -std=c++17 -pthread -Wall
AUX_HEADERS = stringhelper.h Safe.h SimulatorUserInput.h
PEER_HEADERS = PeerConnection.h SenderSocket.h ReceiverSocket.h
CLIENT_HEADERS = ${PEER_HEADERS} Client.h
SERVER_HEADERS = ${PEER_HEADERS} Server.h
PACK_HEADERS = Packager.h Unpackager.h UnpackagerMaster.h VerifierPacks.h
CONNECTIONS_HEADERS = MasterConnection.h SlaveClientConnection.h SlaveServerConnection.h SlaveConnections.h

AUX_HEADERS := $(addprefix aux/, $(AUX_HEADERS))
CLIENT_HEADERS := $(addprefix peer/, $(CLIENT_HEADERS))
SERVER_HEADERS := $(addprefix peer/, $(SERVER_HEADERS))
PACK_HEADERS := $(addprefix Pack/, $(PACK_HEADERS))
CONNECTIONS_HEADERS := $(addprefix Connections/, $(CONNECTIONS_HEADERS))
MY_HEADERS = Database.h globals.h

all: slave.exe
	@echo Terminado!!

slave.exe: slave.cpp ${AUX_HEADERS} ${CLIENT_HEADERS} ${SERVER_HEADERS} ${PACK_HEADERS} ${CONNECTIONS_HEADERS} ${MY_HEADERS}
	g++ slave.cpp -o slave.exe ${FLAGS}

debug: slave.cpp ${AUX_HEADERS} ${CLIENT_HEADERS} ${SERVER_HEADERS} ${PACK_HEADERS} ${CONNECTIONS_HEADERS} ${MY_HEADERS}
	g++ slave.cpp -o slave.exe ${FLAGS} -g

clear:
	rm ./slave.exe
