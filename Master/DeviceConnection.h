#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H 
#include <iostream>
#include <vector>
#include "peer/Server.h"
#include "peer/Client.h"
#include "SlaveConnection.h"
#include "auxPackCmd.h"
#include "globals.h"
using namespace std;

class DeviceConnection: public PeerConnection
{
private:
	enum phaseSystem{PHASE_SETUP, PHASE_RUN};
	enum typePack{PACK_UNICAST, PACK_MULTICAST};

public:
	phaseSystem currentPhase;
	VerifierCmd verifiercmd;
	CreatorPack creatorpack;
	MasterExtractorPK extractorpack;
public:
	int k; // k = slaves.size();
	std::vector< Client<SlaveConnection>* > slaves;
	std::vector<bool> isAlive;

public:
	DeviceConnection(int socketFD): PeerConnection(socketFD)
	{
	    currentPhase = PHASE_SETUP;
	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		string cmd = receiveCommandFromClient();
		if(cmd.empty()){
			printf("Conexion cerrada\n");
			closeConnection();
			return;
		}
		typeCmd typecmd = verifiercmd.getTypeOfCmd(cmd);
		bool correctPhase = isCorrectPhase(typecmd);
		if(correctPhase)
		{
			switch(typecmd)
			{
				case CMD_CONNECT:processConnect(cmd); break;
				case CMD_START:  processStart();      break;
				case CMD_CREATE: processOnePk (cmd,  typecmd,  PACK_MULTICAST); break;
				case CMD_LINK:   processTwoPks(cmd,  typecmd,  PACK_MULTICAST); break;
				case CMD_DELETE: processOnePk (cmd,  typecmd,  PACK_MULTICAST); break;
				case CMD_UNLINK: processTwoPks(cmd,  typecmd,  PACK_MULTICAST); break;
				case CMD_UPDATE: processOnePk (cmd,  typecmd,  PACK_UNICAST); break;
				case CMD_EXPLORE:processOnePk (cmd,  typecmd,  PACK_UNICAST); break;
				case CMD_SELECT: processOnePk (cmd,  typecmd,  PACK_UNICAST); break;
				case CMD_ERROR:  processError(); break;
			}
		}
		else{
			sendMessageToClient("NOO, COMMAND AT INCORRECT PHASE");
		}
	}
	
private:
	// Los casos de uso implementados
	void processConnect(string cmd);
	void processStart();
	void processOnePk(string cmd, typeCmd typecmd, typePack typepack);
	void processTwoPks(string cmd, typeCmd typecmd, typePack typepack);
	void processError();
	void keepAlive();


	// comunicaciones con Device y Slaves
	void sendPackToSlave(int slaveid, string pack);
	void sendMessageToClient(string msg);
	string receiveMessageFromSlave(int slaveid);
	string receiveCommandFromClient();

	// comunicaciones avanzadas
	void broadcastAllSlaves(string pack);
	void multicastTwoSlaves(int slaveid, string pack);
	void unicastSlave(int slaveid, string pack);

	// funciones auxiliares
	int chooseSlave(string pk);
	bool isCorrectPhase(typeCmd typecmd);
	void setAllSlavesAlive();
	void startThreadKeepAlive();
	int nextSlaveId(int slaveid);
	bool isPingMsg(string msg);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DeviceConnection::processConnect(string cmd)
{
	vector<string> tokens = simpleTokenizer(cmd);
	string ip = tokens[1];
	int port = stoi(tokens[2]);

	Client<SlaveConnection>* slave = new Client<SlaveConnection>();
	bool isConnected = slave->newThread_connectToServer(ip, port);
	if(isConnected){
		slaves.push_back(slave); 
		sendMessageToClient("THE SLAVE WAS CONNECTED!");
	}
	else {
		sendMessageToClient("ERROR CONNECTING SLAVE");
	}
}

void DeviceConnection::processStart()
{
	k = slaves.size();
	if(k > 0) {
		currentPhase = PHASE_RUN;
		setAllSlavesAlive();
		startThreadKeepAlive();
		string pack = creatorpack.createPackListOfSlavesIps(slaves);
		broadcastAllSlaves(pack);
		sendMessageToClient("THE SYSTEM IS UP");
	}
	else sendMessageToClient("ERROR AT START");
}

void DeviceConnection::processOnePk(string cmd, typeCmd typecmd, typePack typepack)
{
	string pack = creatorpack.createPack(typecmd, cmd);
	string pk = extractorpack.extractOnePk(typecmd, cmd);
	int slaveid = chooseSlave(pk);
	if(typepack == PACK_MULTICAST)
		multicastTwoSlaves(slaveid, pack);
	else 
		unicastSlave(slaveid, pack);
}

void DeviceConnection::processTwoPks(string cmd, typeCmd typecmd, typePack typepack)
{
	string pk1, pk2;
	string pack = creatorpack.createPack(typecmd, cmd); 
	tie(pk1, pk2) = extractorpack.extractTwoPks(typecmd, cmd);
	int slaveid1 = chooseSlave(pk1);
	int slaveid2 = chooseSlave(pk2);
	if(typepack == PACK_MULTICAST){
		multicastTwoSlaves(slaveid1, pack);
		multicastTwoSlaves(slaveid2, pack);
	}
	else {
		unicastSlave(slaveid1, pack);
		unicastSlave(slaveid2, pack);
	}
}

void DeviceConnection::processError()
{
	sendMessageToClient("INCORRECT FORMAT");
}


void DeviceConnection::keepAlive()
{
	while(true)
	{
		cout << "Broadcast pings" << endl;
		string pingpack = creatorpack.createPackPing();
		broadcastAllSlaves(pingpack);
		for(Client<SlaveConnection>* slave:slaves){
			slave->getInstanceOfPeerConnection()->setPingResponse(false);
		}
		sleep(1); // Espera un segundo
		for(int i=0; i<k; ++i){
			SlaveConnection* myslave = slaves[i]->getInstanceOfPeerConnection();
			if(isAlive[i] && myslave->timeout()) 
			{
				cout << "slave " << i << " ahora no esta vivo" << endl;
				isAlive[i] = false;
				myslave->closeConn();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DeviceConnection::sendPackToSlave(int slaveid, string pack)
{
	slaves[slaveid]->getInstanceOfPeerConnection()->sendPack(pack);
}

void DeviceConnection::sendMessageToClient(string msg)
{
	sender.sendStr(msg + '\n');
}

string DeviceConnection::receiveMessageFromSlave(int slaveid)
{
	return slaves[slaveid]->getInstanceOfPeerConnection()->receiveMessage();
}

string DeviceConnection::receiveCommandFromClient()
{
	string cmd;
	for(char c = receiver.recvChar(); c!='\0' && c!='\n'; c = receiver.recvChar())
		cmd += c;
	return cmd;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DeviceConnection::broadcastAllSlaves(string pack)
{
	for(int i=0; i<k; ++i){ 
		if(isAlive[i])
			slaves[i]->getInstanceOfPeerConnection()->sendPack(pack);
	}
}


void DeviceConnection::multicastTwoSlaves(int slaveid, string pack)
{
	unicastSlave(slaveid, pack);
	unicastSlave(nextSlaveId(slaveid), pack);
}


void DeviceConnection::unicastSlave(int slaveid, string pack)
{
	if(isAlive[slaveid]){
		sendPackToSlave(slaveid, pack);
		string msg = receiveMessageFromSlave(slaveid);
		sendMessageToClient(msg);
	}
	else {
		sendMessageToClient("THE SLAVE IS OFFLINE");
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Puede presentar problemas!!, alerta, solo esta pensado para letras de la [a-z]?
// fuente: https://cp-algorithms.com/string/string-hashing.html
int DeviceConnection::chooseSlave(string pk)
{
    const int p = 31;
    const int m = 1e9 + 9;
    long long hash_value = 0;
    long long p_pow = 1;
    for (char c : pk) {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    } 
    hash_value = abs(hash_value)%k;
	return hash_value;
}


bool DeviceConnection::isCorrectPhase(typeCmd typecmd)
{
	if(typecmd==CMD_CONNECT || typecmd==CMD_START){
		if(currentPhase == PHASE_SETUP)
			 return true;
		else return false;
	}
	else { // Cualquier otro comando
		if(currentPhase == PHASE_RUN)
			return true;
		else return false;
	}
}

void DeviceConnection::setAllSlavesAlive()
{
	isAlive.assign(k, true);
}
void DeviceConnection::startThreadKeepAlive()
{
	thread th(&DeviceConnection::keepAlive, this);
	th.detach();
}

int  DeviceConnection::nextSlaveId(int slaveid)
{
	return (slaveid+1)%k;
}


#endif