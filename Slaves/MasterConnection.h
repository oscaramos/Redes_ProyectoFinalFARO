#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H 
#include <iostream>
#include <vector>
#include <map>
#include "peer/Server.h"
#include "peer/Client.h"
#include "SlaveClientConnection.h"
#include "auxPackCmd.h"
#include "globals.h"
using namespace std;

class MasterConnection: public PeerConnection
{
private:
	//enum typeModePack{PACK_UNICAST, PACK_MULTICAST};

public:
	VerifierMasterPack verifierpack;
	CreatorSlavePack creatorpack;
// conexiones
public:
	int k; // k = slaves.size();
	std::vector< Client<SlaveClientConnection>* > slavesclients;
	std::vector<bool> isAlive;
// grafo


public:
	MasterConnection(int socketFD): PeerConnection(socketFD)
	{

	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		char firstchar = receiver.recvChar();
		receiver.ignore(1);
		if(firstchar == '\0'){
			printf("Conexion con usuario cerrada\n");
			closeConnection();
			return;
		}

		string message;
		typeMasterPack typepack = verifierpack.getTypeOfPack(firstchar);
		switch(typepack)
		{
			case PCKMASTER_START:  message=processStart();  break;
			case PCKMASTER_CREATE: message=processCreate(); break;
			case PCKMASTER_LINK:   message=processLink();   break;
			case PCKMASTER_DELETE: message=processDelete ();break;
			case PCKMASTER_UNLINK: message=processUnlink(); break;
			// case PCKMASTER_UPDATE: processOnePk (cmd,  typepack,  PACK_UNICAST); break;
			// case PCKMASTER_EXPLORE:processOnePk (cmd,  typepack,  PACK_UNICAST); break;
			// case PCKMASTER_SELECT: processOnePk (cmd,  typepack,  PACK_UNICAST); break;
			case PCKMASTER_ERROR:  message=processError(); break;
		}
		message = intWithZeros(message.size(), 3) + message;
		sendMessageToMaster(message);
	}
		
private:
	// Los casos de uso implementados
	string processStart();
	string processCreate();
	string processLink();
	string processDelete();
	string processUnlink();
	string processError();

	// comunicaciones con Master y Slaves
	void sendMessageToMaster(string msg);
	void sendPackToSlave(int slaveid, string pack);
	bool receivePackTrueOrFalseFromSlave(int slaveid);

	// comunicaciones internas Slaves y Slaves
	bool existsNode(string pk);

	// funciones auxiliares
	int chooseSlave(string pk);
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ejemplo: [0 02 08 10.0.0.1 08 10.0.0.2]
// estructura: [1, 2 [2 var]+]
string MasterConnection::processStart()
{
	// recibir ips_slaves
	int lenlist = receiver.recvInt(2); receiver.ignore(1);
	for(int i=0; i<lenlist; ++i){
		string ip_slave = receiver.recvField(2);	
		// conectar con slaves
		Client<SlaveClientConnection>* client = new Client<SlaveClientConnection>();
		bool isconnected = client->newThread_connectToServer(ip_slave, port_slave);
		if(!isconnected)
			cout << "conexion fallida con: " << ip_slave << endl;
		slavesclients.push_back(client);
	}
	k = lenlist;
	return "p"; 
}

// ejemplo: [1 03 C:\ 018 size: 12, files: 2]
// estructura: [1,2 var,3 var] 
string MasterConnection::processCreate()
{
	string pk = receiver.recvField(2);
	string content = receiver.recvField(3);
	if(!existsNode(pk)){
		database.createNode(pk, content);
		return "THE NODE WAS CREATED!";
	}
	else
		return "THE NODE ALREADY EXISTS";
}

// ejemplo: [2 03 C:\ 12 C:\proyectos]
// estructura: [1, 2 var, 2 var]
string MasterConnection::processLink()
{
	string pk1 = receiver.recvField(2);
	string pk2 = receiver.recvField(2);
	if(existsNode(pk1) && existsNode(pk2)){
		if(existsNode(pk1)) database.linkRelationship(pk1, pk2);
		else                database.linkRelationship(pk2, pk1);
		return "THE NODES WERE LINKED!";
	}
	else{
		return "SOME NODES DON’T EXIST";
	}
}

// ejemplo: [3 12 C:/proyectos]
// estructura: [1, 2 var]
string MasterConnection::processDelete()
{
	string pk = receiver.recvField(2);
	if(existsNode(pk)){
		// primero borro todas las relaciones
		// for(string rel: database.getAllRelationships(pk))
		// {
		// 	;
		// }
		database.deleteNode(pk);
		return "THE NODE WAS DELETED!";
	}
	else
		return "THE NODE ALREADY EXISTS";
}


// ejemplo: [4 03 C:\ 12 C:\proyectos]
// estructura: [1, 2 var, 2 var]
string MasterConnection::processUnlink()
{
	string pk1 = receiver.recvField(2);
	string pk2 = receiver.recvField(2);
	if(existsNode(pk1) && existsNode(pk2)){
		if(existsNode(pk1)) database.unlinkRelationship(pk1, pk2);
		else                database.unlinkRelationship(pk2, pk1);
		return "THE NODES WERE LINKED!";
	}
	else{
		return "SOME NODES DON’T EXIST";
	}
}



string MasterConnection::processError()
{
	return "INCORRECT FORMAT";
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MasterConnection::sendMessageToMaster(string msg)
{
	sender.sendStr(msg);
}

void MasterConnection::sendPackToSlave(int slaveid, string pack)
{
	slavesclients[slaveid]->getInstanceOfPeerConnection()->sendPack(pack);
}

bool MasterConnection::receivePackTrueOrFalseFromSlave(int slaveid)
{
	return slavesclients[slaveid]->getInstanceOfPeerConnection()->receivePackTrueOrFalse();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool MasterConnection::existsNode(string pk)
{
	string pack = creatorpack.createPack(PCKSLAVE_EXIST, {pk});
	int slaveid = chooseSlave(pk);
	sendPackToSlave(slaveid, pack);
	bool exist = receivePackTrueOrFalseFromSlave(slaveid);
	return exist;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Puede presentar problemas!!, alerta, solo esta pensado para letras de la [a-z]?
// fuente: https://cp-algorithms.com/string/string-hashing.html
int MasterConnection::chooseSlave(string pk)
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


#endif