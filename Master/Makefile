FLAGS = -std=c++11 -pthread -Wall
AUX_HEADERS = stringhelper.h Safe.h SimulatorUserInput.h
PEER_HEADERS = PeerConnection.h SenderSocket.h ReceiverSocket.h
CLIENT_HEADERS = ${PEER_HEADERS} Client.h

AUX_HEADERS := $(addprefix aux/, $(AUX_HEADERS))
CLIENT_HEADERS := $(addprefix peer/, $(CLIENT_HEADERS))
MY_HEADERS = auxPackCmd.h DeviceConnection.h SlaveConnection.h globals.h

all: master.exe
	@echo Terminado!!

master.exe: master.cpp ${AUX_HEADERS} ${CLIENT_HEADERS} ${MY_HEADERS}
	g++ master.cpp -o master.exe ${FLAGS}

debug: master.cpp ${AUX_HEADERS} ${CLIENT_HEADERS} ${MY_HEADERS}
	g++ master.cpp -o master.exe ${FLAGS} -g

clear:
	rm ./master.exe
