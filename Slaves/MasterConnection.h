#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H 
#include <iostream>
#include <vector>
#include <map>
#include "peer/Server.h"
#include "peer/Client.h"
#include "SlaveConnections.h"
#include "globals.h"
#include "VerifierPacks.h"
#include "CreatorPacks.h"
using namespace std;

class MasterConnection: public PeerConnection
{
public:
	VerifierMasterPack verifierpack;
	CreatorSlavePack creatorpack;

public:
	
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
		// NUNCA PONER UN .IGNORE(1) AQUI, JAMAS!!!
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
			case PCKMASTER_EXPLORE:message=processExplore(); break;
			case PCKMASTER_SELECT: message=processSelect(); break;
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
	string processExplore();
	string processSelect();
	string processPing();
	string processError();

	// comunicaciones con Master y Slaves
	void sendMessageToMaster(string msg);

	// comunicaciones internas Slaves y Slaves
	bool existsNodeInSlaves(string pk);
	vector<pair<int,string>> exploreInSlaves(string pk, int prof);
	vector<pair<int,pair<string,string>>> selectInSlaves(string pk, int prof);

	// base de datos aqui
	bool existsNodeHere(string pk);
	bool existLinkHere(string pk1, string pk2);
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
		bool isconnected = client->newThread_connectToServer(ip_slave, port_slave+(debugMode?i:0)); // es temporal el +1, quitar en produccion 
		if(!isconnected)
			cout << "START: conexion fallida con: " << ip_slave << endl;
		else
			slavesconn.slavesclients.push_back(client);
	}
	slavesconn.k = lenlist;
	cout << "START: k = " << slavesconn.k << endl;
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


// ejemplo: [6 03 C:\ 05]
// estructura: [1, 2 var, 2]
string MasterConnection::processExplore()
{
	cout << "MasterConnection::Explore: iniciado" << endl;
	string message;
	receiver.ignore(1); string pk = receiver.recvField(2);
	receiver.ignore(1); int prof = receiver.recvInt(2);
	cout << "MasterConnection::Explore: pk="<<pk<<" prof="<<prof<<endl;

	vector<pair<int,string>> explored = exploreInSlaves(pk, prof);
	reverse(explored.begin(), explored.end());
	for(pair<int,string> p: explored){
		cout << "MasterConnection::DATA: rawprof="<<p.first<<" pk="<<p.second<<endl;
		message += string(2*(prof-p.first), '-') + ">" + p.second + "\n";
	}
	message.erase(message.end()-1); // para evitar falso positivo de cerrado al cliente
	return message;
}

// ejemplo: [7 03 C:\ 05]
// estructura: [1, 2 var, 2]
string MasterConnection::processSelect()
{
	cout << "MasterConnection::Select: iniciado" << endl;
	string message;
	receiver.ignore(1); string pk = receiver.recvField(2);
	receiver.ignore(1); int prof = receiver.recvInt(2);
	cout << "MasterConnection::Select: pk="<<pk<<" prof="<<prof<<endl;

	vector<pair<int,pair<string,string>>> explored = selectInSlaves(pk, prof); // retorna pair<prof, pk, content>
	reverse(explored.begin(), explored.end());
	for(pair<int,pair<string,string>> p: explored){
		cout << "MasterConnection::DATA: rawprof="<<p.first<<" pk="<<p.second.first<<" content="<<p.second.second<<endl;
		message += string(2*(prof-p.first), '-') + ">" + p.second.first + "\t("+p.second.second+")" + "\n";
	}
	message.erase(message.end()-1); // para evitar falso positivo de cerrado al cliente, quito la ultima '\n'
	return message;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: falta multicast
bool MasterConnection::existsNodeInSlaves(string pk)
{
	string pack = creatorpack.createPackExist(PCKSLAVE_EXIST, pk);
	cout << "ExistNode: envio paquete ["<<pack<<"]" << endl;
	int slaveid = slavesconn.chooseSlave(pk);
	cout << "ExistNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	bool exist = slavesconn.receivePackTrueOrFalseFromSlave(slaveid);
	cout << "ExistNode: exist = " << exist << endl;
	return exist;
}


vector<pair<int,string>> MasterConnection::exploreInSlaves(string pk1, int prof)
{
	string pack = creatorpack.createPackExploreRequest(pk1, prof);
	cout << "MasterConnection::exploreInSlaves: envio paquete ["<<pack<<"]" << endl;

	vector<pair<int,string>> explored;
	vector<string> relations = database.getAllRelationships(pk1);
	for(string pk2: relations){
		int slaveid = slavesconn.chooseSlave(pk2);
		cout << "MasterConnection::exploreInSlaves: pk2="<<pk2<<" slaveid="<<slaveid<<endl;
		slavesconn.sendPackToSlave(slaveid, pack);
		vector<pair<int,string>> response = slavesconn.receivePackExplore(slaveid);
		explored = concatVectors(explored, response);
	}
	return explored;
}

vector<pair<int,pair<string,string>>> MasterConnection::selectInSlaves(string pk1, int prof)
{
	string pack = creatorpack.createPackSelectRequest(pk1, prof);
	cout << "MasterConnection::selectInSlaves: envio paquete ["<<pack<<"]" << endl;

	vector<pair<int,pair<string,string>>> explored;
	vector<string> relations = database.getAllRelationships(pk1);
	for(string pk2: relations){
		int slaveid = slavesconn.chooseSlave(pk2);
		cout << "MasterConnection::selectInSlaves: pk2="<<pk2<<" slaveid="<<slaveid<<endl;
		slavesconn.sendPackToSlave(slaveid, pack);
		vector<pair<int,pair<string,string>>> response = slavesconn.receivePackSelect(slaveid);
		explored = concatVectors(explored, response);
	}
	return explored;
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


#endif