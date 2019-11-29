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
		// NUNCA PONER UN .IGNORE(1) AQUI, JAMAS
		if(firstchar == '\0'){
			printf("Conexion con usuario cerrada\n");
			closeConnection();
			return;
		}
		cout << "first char = " << firstchar << endl;

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
			case PCKMASTER_PING:   message=processPing(); break;
			case PCKMASTER_ERROR:  message=processError(); break;
		}
		message = intWithZeros(message.size(), 3) + " " + message;
		cout << "Send master message = " << message << endl;
		sendMessageToMaster(message);
		cout << endl;
	}
		
private:
	// Los casos de uso implementados
	string processStart();
	string processCreate();
	string processLink();
	string processDelete();
	string processUnlink();
	string processPing();
	string processError();

	// comunicaciones con Master y Slaves
	void sendMessageToMaster(string msg);
	void sendPackToSlave(int slaveid, string pack);
	bool receivePackTrueOrFalseFromSlave(int slaveid);

	// comunicaciones internas Slaves y Slaves
	bool existsNodeInSlaves(string pk);

	// base de datos aqui
	bool existsNodeHere(string pk);
	bool existLinkHere(string pk1, string pk2);

	// funciones auxiliares
	int chooseSlave(string pk);
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ejemplo: [0 02 08 10.0.0.1 08 10.0.0.2]
// estructura: [1, 2 [2 var]+]
string MasterConnection::processStart()
{
	receiver.ignore(1);
	// recibir ips_slaves
	cout << "START: iniciado" << endl;
	int lenlist = receiver.recvInt(2);
	cout << "START: son " << lenlist << " ips" << endl;
	for(int i=0; i<lenlist; ++i){
		receiver.ignore(1);
		string ip_slave = receiver.recvField(2);	
		cout << "START: Conectando con ip " << ip_slave << " y puerto " << port_slave << endl; // slaveid es de debug
		// conectar con slaves
		Client<SlaveClientConnection>* client = new Client<SlaveClientConnection>();
		bool isconnected = client->newThread_connectToServer(ip_slave, port_slave+i); // es temporal el +1, quitar en produccion 
		if(!isconnected)
			cout << "START: conexion fallida con: " << ip_slave << endl;
		else
			slavesclients.push_back(client);
	}
	k = lenlist;
	cout << "START: terminado" << endl;
	return "p"; 
}

// ejemplo: [1 03 C:\ 018 size: 12, files: 2]
// estructura: [1,2 var,3 var] 
string MasterConnection::processCreate()
{
	cout << "Create: iniciado" << endl;
	receiver.ignore(1);
	string pk = receiver.recvField(2);
	receiver.ignore(1);
	string content = receiver.recvField(3);
	if(!existsNodeHere(pk)){
		cout << "Create: creando nodo" << endl;
		database.createNode(pk, content);
		return "THE NODE WAS CREATED!";
	}
	else{
		cout << "Create: no crea nodo porque ya existe" << endl;
		return "THE NODE ALREADY EXISTS";
	}
}

// ejemplo: [2 03 C:\ 12 C:\proyectos]
// estructura: [1, 2 var, 2 var]
string MasterConnection::processLink()
{
	cout << "Link: iniciado" << endl;
	receiver.ignore(1);
	string pk1 = receiver.recvField(2);
	receiver.ignore(1);
	string pk2 = receiver.recvField(2);
	cout << "Link: pk1="<<pk1<<" pk2="<<pk2<< endl;
	if(existsNodeInSlaves(pk1) && existsNodeInSlaves(pk2)){
		bool isLinked = false;
		if(existsNodeHere(pk1)){
			if(!existLinkHere(pk1, pk2)){
				cout << "Link: linkeando nodos" << endl;
				database.linkRelationship(pk1, pk2);
				isLinked = true;
			}
			else {
				cout << "Link: ya existe link" << endl;
			}
		}
		if(existsNodeHere(pk2)){
			if(!existLinkHere(pk2, pk1)){
				cout << "Link: linkeando nodos" << endl;
				database.linkRelationship(pk2, pk1);
				isLinked = true;
			}
			else {
				cout << "Link: ya existe link" << endl;
			}
		}
		if(isLinked)
			return "THE NODES WERE LINKED!";
		else 
			return "THE LINK ALREADY EXISTS!";
	}
	else{
		return "SOME NODES DON’T EXIST";
	}
}

// ejemplo: [3 12 C:/proyectos]
// estructura: [1, 2 var]
string MasterConnection::processDelete()
{
	receiver.ignore(1);
	string pk = receiver.recvField(2);
	if(existsNodeHere(pk)){
		// primero borro todas las relaciones
		// for(string rel: database.getAllRelationships(pk))
		// {
		// 	;
		// }
		database.deleteNode(pk);
		return "THE NODE WAS DELETED!";
	}
	else
		return "THE NODE DON'T EXIST";
}


// ejemplo: [4 03 C:\ 12 C:\proyectos]
// estructura: [1, 2 var, 2 var]
string MasterConnection::processUnlink()
{
	cout << "Unlink: iniciado" << endl;
	receiver.ignore(1);
	string pk1 = receiver.recvField(2);
	receiver.ignore(1);
	string pk2 = receiver.recvField(2);
	cout << "Unlink: pk1="<<pk1<<" pk2="<<pk2<< endl;
	if(existsNodeInSlaves(pk1) && existsNodeInSlaves(pk2)){
		bool isUnlinked = false;
		if(existsNodeHere(pk1)){
			if(existLinkHere(pk1, pk2)){
				cout << "Unlink: deslinkeando nodos" << endl;
				database.unlinkRelationship(pk1, pk2);
				isUnlinked = true;
			}
			else {
				cout << "Unlink: no existe link" << endl;
			}
		}
		if(existsNodeHere(pk2)){
			if(existLinkHere(pk2, pk1)){
				cout << "Unlink: deslinkeando nodos" << endl;
				database.unlinkRelationship(pk2, pk1);
				isUnlinked = true;
			}
			else {
				cout << "Unlink: no existe link" << endl;
			}
		}
		if(isUnlinked)
			return "THE NODES WERE UNLINKED!";
		else 
			return "THE LINK DON'T EXIST!";
	}
	else{
		return "SOME NODES DON’T EXIST";
	}
}

// ejemplo: [p]
// estructura: [cmd] 
string MasterConnection::processPing()
{
	return "p";
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

// TODO: falta multicast
bool MasterConnection::existsNodeInSlaves(string pk)
{
	string pack = creatorpack.createPack(PCKSLAVE_EXIST, {pk});
	cout << "ExistNode: envio paquete ["<<pack<<"]" << endl;
	int slaveid = chooseSlave(pk);
	cout << "ExistNode: al slave " << slaveid << endl;
	sendPackToSlave(slaveid, pack);
	bool exist = receivePackTrueOrFalseFromSlave(slaveid);
	cout << "ExistNode: exist = " << exist << endl;
	return exist;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool MasterConnection::existsNodeHere(string pk)
{
	return database.existNode(pk);
}

bool MasterConnection::existLinkHere(string pk1, string pk2)
{
	return database.existLink(pk1, pk2);
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