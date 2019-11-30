#ifndef SLAVESERVERCONNECTION_H
#define SLAVESERVERCONNECTION_H
#include <iostream>
#include <queue>
#include "SlaveConnections.h"
#include "peer/Client.h"
#include "globals.h"
#include "VerifierPacks.h"
#include "CreatorPacks.h"
using namespace std;


class SlaveServerConnection: public PeerConnection
{
private:
	VerifierSlavePack verifierpack;
	CreatorSlavePack creatorpack;

public:
	SlaveServerConnection(int socketFD): PeerConnection(socketFD)
	{
	    ;
	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		char firstchar = receiver.recvChar();
		if(firstchar == '\0'){
			printf("Conexion con slaves cerrada\n");
			closeConnection();
			return;
		}

		string pack;
		typeSlavePack typepack = verifierpack.getTypeOfPack(firstchar);
		switch(typepack)
		{
			case PCKSLAVE_EXIST:   pack=processExist();   break;
			case PCKSLAVE_EXPLORE: pack=processExplore(); break;
			case PCKSLAVE_SELECT:  pack=processSelect(); break;
			case PCKSLAVE_ERROR:   pack=processError();   break;
			default: break;
		}
		cout << "SlaveServerConnection: envio paquete = ["<<pack<<"]" << endl;
		sender.sendStr(pack);
	}

	string processExist();
	string processExplore();
	string processSelect();
	string processError();
};

// ejemplo: 1 03 C:/
// estructura: [cmd, len_pk, pk]
// longitudes: [1, 2 var]
string SlaveServerConnection::processExist()
{
	cout << "processExist: Inicia" << endl;
	receiver.ignore(1); string pk = receiver.recvField(2);
	cout << "processExist: PK = " << pk << endl;
	if(database.existNode(pk))
		return creatorpack.createPack(PCKSLAVE_TRUE);
	else 
		return creatorpack.createPack(PCKSLAVE_FALSE);
}

// ejemplo: 2 03 C:/ 05 02 01 C 02 C:
// estructura: [cmd, len_pk, pk, prof, len_list, [len_pk, pk]+]
// longitudes: [1, 2 var, 2, 2 [2, var]+] 
string SlaveServerConnection::processExplore()
{
	cout << "SlaveServerConnection::processExplore: Inicia" << endl;
	receiver.ignore(1); string pk1 = receiver.recvField(2);
	receiver.ignore(1); int prof = receiver.recvInt(2);
	receiver.ignore(1); int len_list = receiver.recvInt(2);
	cout << "SlaveServerConnection::processExplore: pk="<<pk1<<" prof="<<prof<<" lenlist="<<len_list<<endl;
	vector<string> new_visited;
	for(int i=0; i<len_list; ++i){
		receiver.ignore(1); string pk = receiver.recvField(2);
		new_visited.push_back(pk);
	}
	new_visited.push_back(pk1);
	vector<pair<int, string>> explored;
	if(prof > 0){
		for(string pk2: database.getAllRelationships(pk1)){
			if(find(new_visited.begin(), new_visited.end(), pk2) != new_visited.end()) continue; // No visito los ya visitados
			string pack = creatorpack.createPackExploreRequest(pk2, prof-1, new_visited);
			int slaveid = slavesconn.chooseSlave(pk2);
			cout << "SlaveServerConnection::processExplore: pk2="<<pk2<<" slaveid="<<slaveid<<endl;
			cout << "SlaveServerConnection::processExplore: Envio pack["<<pack<<"]" << endl;
			slavesconn.sendPackToSlave(slaveid, pack);
			vector<pair<int, string>> response = slavesconn.receivePackExplore(slaveid);
			explored = concatVectors(explored, response);
		}
	}
	explored.push_back(make_pair(prof, pk1));
	return creatorpack.createPackExploreResponse(explored);
}


// casi igual que el explore
// ejemplo: 4 03 C:/ 05 02 01 C 02 C:
// estructura: [cmd, len_pk, pk, prof, len_list, [len_pk, pk]+]
// longitudes: [1, 2 var, 2, 2 [2, var]+] 
string SlaveServerConnection::processSelect()
{
	cout << "SlaveServerConnection::processSelect: Inicia" << endl;
	receiver.ignore(1); string pk1 = receiver.recvField(2);
	receiver.ignore(1); int prof = receiver.recvInt(2);
	receiver.ignore(1); int len_list = receiver.recvInt(2);
	cout << "SlaveServerConnection::processSelect: pk="<<pk1<<" prof="<<prof<<" lenlist="<<len_list<<endl;
	vector<string> new_visited;
	for(int i=0; i<len_list; ++i){
		receiver.ignore(1); string pk = receiver.recvField(2);
		new_visited.push_back(pk);
	}
	new_visited.push_back(pk1);
	vector<pair<int, pair<string, string>>> explored;
	if(prof > 0){
		for(string pk2: database.getAllRelationships(pk1)){
			if(find(new_visited.begin(), new_visited.end(), pk2) != new_visited.end()) continue; // No visito los ya visitados
			string pack = creatorpack.createPackSelectRequest(pk2, prof-1, new_visited);
			int slaveid = slavesconn.chooseSlave(pk2);
			cout << "SlaveServerConnection::processSelect: pk2="<<pk2<<" slaveid="<<slaveid<<endl;
			cout << "SlaveServerConnection::processSelect: Envio pack["<<pack<<"]" << endl;
			slavesconn.sendPackToSlave(slaveid, pack);
			vector<pair<int, pair<string, string>>> response = slavesconn.receivePackSelect(slaveid);
			explored = concatVectors(explored, response);
		}
	}
	explored.push_back(make_pair(prof, make_pair(pk1, database.getContent(pk1))));
	return creatorpack.createPackSelectResponse(explored);
}

string SlaveServerConnection::processError()
{
	cout << "SlaveServerConnection: error paquete no reconocido" << endl;
	exit(0);
}


#endif