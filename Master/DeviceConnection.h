#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H 
#include <iostream>
#include "peer/Server.h"
#include "auxPackCmd.h"
#include "globals.h"
using namespace std;

class DeviceConnection: public PeerConnection
{
private:
	enum phaseSystem{PHASE_SETUP, PHASE_RUN};

public:
	phaseSystem currentPhase;
	VerifierCmd verifiercmd;
	CreatorPack creatorpack;
	MasterExtractorPK extractorpack;

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
		if(!cmd.empty())
		{
			typeCmd typecmd = verifiercmd.getTypeOfCmd(cmd);
			bool correctPhase = isCorrectPhase(typecmd);
			if(correctPhase)
			{
				switch(typecmd)
				{
					case CMD_CONNECT:processConnect(cmd); break;
					case CMD_START:  processStart();      break;
					case CMD_CREATE: processCreate(cmd);  break;
					case CMD_LINK:   processLink(cmd);    break;
					case CMD_DELETE: processDelete(cmd);  break;
					case CMD_UNLINK: processUnlink(cmd);  break;
					case CMD_UPDATE: processUpdate(cmd);  break;
					case CMD_ERROR: processError(); break;
				}
			}
			else 
			{
				sendMessageToClient("NOO, COMMAND AT INCORRECT PHASE");
			}
		}
		else 
		{
			printf("Conexion cerrada\n");
			closeConnection();
		}
	}
	
private:
	// Los casos de uso implementados
	void processConnect(string cmd);
	void processStart();
	void processCreate(string cmd);
	void processLink(string cmd);
	void processDelete(string cmd);
	void processUnlink(string cmd);
	void processUpdate(string cmd);
	void processError();

	// funciones auxiliares
	int chooseSlave(string pk);
	void sendPackToSlave(int slaveid, string pack);
	void sendMessageToClient(string msg);
	string receiveMessageFromSlave(int slaveid);
	string receiveCommandFromClient();
	bool isCorrectPhase(typeCmd typecmd);
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
		slaves.push_back(slave); // move hace que no se cree una copia
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
		sendMessageToClient("THE SYSTEM IS UP");
	}
	else sendMessageToClient("ERROR AT START");
}

void DeviceConnection::processCreate(string cmd)
{
	string pack = creatorpack.createPack(CMD_CREATE, cmd);
	string pk = extractorpack.extractOnePk(CMD_CREATE, cmd);
	int slaveid = chooseSlave(pk);
	sendPackToSlave(slaveid, pack);
	string msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);
}


void DeviceConnection::processLink(string cmd)
{
	string pack, pk1, pk2;
	pack = creatorpack.createPack(CMD_LINK, cmd); 
	tie(pk1, pk2) = extractorpack.extractTwoPks(CMD_LINK, cmd);
	
	string msg;
	int slaveid; 
	slaveid = chooseSlave(pk1);
	sendPackToSlave(slaveid, pack);
	msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);

	slaveid = chooseSlave(pk2);
	sendPackToSlave(slaveid, pack);
	msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);
}

void DeviceConnection::processDelete(string cmd)
{
	string pack = creatorpack.createPack(CMD_DELETE, cmd);
	string pk = extractorpack.extractOnePk(CMD_DELETE, cmd);
	int slaveid = chooseSlave(pk);
	sendPackToSlave(slaveid, pack);
	string msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);
}

void DeviceConnection::processUnlink(string cmd)
{
	string pack, pk1, pk2;
	pack = creatorpack.createPack(CMD_UNLINK, cmd); 
	tie(pk1, pk2) = extractorpack.extractTwoPks(CMD_UNLINK, cmd);
	
	string msg;
	int slaveid; 
	slaveid = chooseSlave(pk1);
	sendPackToSlave(slaveid, pack);
	msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);

	slaveid = chooseSlave(pk2);
	sendPackToSlave(slaveid, pack);
	msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);
}

void DeviceConnection::processUpdate(string cmd)
{
	string pack = creatorpack.createPack(CMD_UPDATE, cmd);
	string oldpk = extractorpack.extractOnePk(CMD_UPDATE, cmd);
	int slaveid = chooseSlave(oldpk);
	sendPackToSlave(slaveid, pack);
	string msg = receiveMessageFromSlave(slaveid);
	sendMessageToClient(msg);
}

void DeviceConnection::processError()
{
	sendMessageToClient("INCORRECT FORMAT");
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

#endif