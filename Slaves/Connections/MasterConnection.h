#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H 
#include <iostream>
#include <vector>
#include <map>
#include "SlaveConnections.h"
#include "../globals.h"
#include "../Pack/VerifierPacks.h"
#include "../Pack/Packager.h"
#include "../Pack/UnpackagerMaster.h"
using namespace std;

class MasterConnection: public PeerConnection
{
public:
	VerifierMasterPack verifierpack;
	Packager packager;
	UnpackagerMaster unpackager;

public:
	MasterConnection(int socketFD): PeerConnection(socketFD), unpackager(socketFD)
	{

	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		char packid = receiver.recvChar(); // NUNCA PONER UN .IGNORE(1) AQUI, JAMAS!!!
		if(packid == '\0'){
			printf("Conexion con usuario cerrada\n");
			closeConnection();
			return;
		}
		cout << "packid = " << packid << endl;

		string message;
		typeMasterPack typepack = verifierpack.getTypeOfPack(packid);
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
	cout << "START: iniciado" << endl;
	vector<string> ip_slaves;
	ip_slaves = unpackager.unpackageStart(); 
	cout << "START: son " << ip_slaves.size() << " ips" << endl;
	slavesconn.k = ip_slaves.size();
	cout << "START: k = " << slavesconn.k << endl;
	for(unsigned int slaveid=0; slaveid<ip_slaves.size(); ++slaveid)
	{
		string ip = ip_slaves[slaveid];
		cout << "START: Conectando con ip " << ip << " y puerto " << port_slave << endl;
		Client<SlaveClientConnection>* client = new Client<SlaveClientConnection>();
		bool isconnected = client->newThread_connectToServer(ip, port_slave+(debugMode?slaveid:0)); 
		if(isconnected){
			slavesconn.slavesclients.push_back(client);
		}
		else{
			cout << "START: conexion fallida con: " << ip << endl;
			exit(0); // termino el programa!!
		}
	}

	for(unsigned int slaveid=0; slaveid<ip_slaves.size(); ++slaveid)
	{
		string pack = packager.packageStart(slaveid);
		cout << "START: envio paquete a slaveid=" << slaveid << " contenido: " << pack << endl;
		slavesconn.sendPackToSlave(slaveid, pack);
	}
	cout << "START: terminado" << endl;
	return "p"; 
}

// ejemplo: [1 03 C:\ 018 size: 12, files: 2]
// estructura: [1,2 var,3 var] 
string MasterConnection::processCreate()
{
	cout << "Create: iniciado" << endl;
	string pk, content;
	tie(pk, content) = unpackager.unpackageCreate();
	cout << "Create: pk=" << pk << " content=" << content << endl;
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
	string pk1, pk2;
	tie(pk1, pk2) = unpackager.unpackageLink();
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
	string pk = unpackager.unpackageDelete();
	cout << "Delete: pk=" << pk << endl;
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
	string pk1, pk2;
	tie(pk1, pk2) = unpackager.unpackageUnlink();
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
	string pk; int prof;
	tie(pk, prof) = unpackager.unpackageExplore();
	cout << "MasterConnection::Explore: pk="<<pk<<" prof="<<prof<<endl;

	string message;
	vector<pair<int,string>> explored = exploreInSlaves(pk, prof);
	reverse(explored.begin(), explored.end());
	for(pair<int,string> p: explored){
		cout << "MasterConnection::DATAExplore: rawprof="<<p.first<<" pk="<<p.second<<endl;
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
	string pk; int prof;
	tie(pk, prof) = unpackager.unpackageSelect();
	cout << "MasterConnection::Select: pk="<<pk<<" prof="<<prof<<endl;

	string message;
	vector<pair<int,pair<string,string>>> explored = selectInSlaves(pk, prof); // retorna pair<prof, pk, content>
	reverse(explored.begin(), explored.end());
	for(pair<int,pair<string,string>> p: explored){
		cout << "MasterConnection::DATASelect: rawprof="<<p.first<<" pk="<<p.second.first<<" content="<<p.second.second<<endl;
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
	string pack = packager.packageExist(pk);
	int slaveid = slavesconn.chooseSlave(pk);
	cout << "ExistNode: envio paquete ["<<pack<<"]" << endl;
	cout << "ExistNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	bool exist = slavesconn.receivePackTrueOrFalseFromSlave(slaveid);
	cout << "ExistNode: exist = " << exist << endl;
	return exist;
}

vector<pair<int,string>> MasterConnection::exploreInSlaves(string pk, int prof)
{
	string pack = packager.packageExploreRequest(pk, prof, {});
	int slaveid = slavesconn.chooseSlave(pk);
	cout << "MasterConnection::exploreInSlaves: envio paquete ["<<pack<<"]" << endl;
	cout << "MasterConnection::exploreInSlaves: pk="<<pk<<" slaveid="<<slaveid<<endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	vector<pair<int,string>> explored = slavesconn.receivePackExplore(slaveid);
	return explored;
}

vector<pair<int,pair<string,string>>> MasterConnection::selectInSlaves(string pk, int prof)
{
	string pack = packager.packageSelectRequest(pk, prof, {});
	int slaveid = slavesconn.chooseSlave(pk);
	cout << "MasterConnection::selectInSlaves: envio paquete ["<<pack<<"]" << endl;
	cout << "MasterConnection::selectInSlaves: pk="<<pk<<" slaveid="<<slaveid<<endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	vector<pair<int,pair<string,string>>> explored = slavesconn.receivePackSelect(slaveid);
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