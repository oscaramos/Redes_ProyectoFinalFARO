#ifndef SLAVESERVERCONNECTION_H
#define SLAVESERVERCONNECTION_H
#include <iostream>
#include <queue>
#include "SlaveConnections.h"
#include "../globals.h"
#include "../Pack/VerifierPacks.h"
#include "../Pack/Packager.h"
#include "../Pack/Unpackager.h"
using namespace std;


class SlaveServerConnection: public PeerConnection
{
private:
	VerifierSlavePack verifierpack;
	Packager packager;
	Unpackager unpackager;

public:
	SlaveServerConnection(int socketFD): PeerConnection(socketFD), unpackager(socketFD)
	{
	    ;
	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		char packid = receiver.recvChar();
		if(packid == '\0'){
			printf("Conexion con slaves cerrada\n");
			closeConnection();
			return;
		}
		cout << "SlaveServerConnection: packid = " << packid;
		typeSlavePack typepack = verifierpack.getTypeOfPack(packid);
		switch(typepack)
		{
			case PCKSLAVE_START:   {thread th(&SlaveServerConnection::processStart, this, unpackager.unpackageStart());           th.detach();} break;
			case PCKSLAVE_EXIST:   {thread th(&SlaveServerConnection::processExist, this, unpackager.unpackageExist());           th.detach();} break;
			case PCKSLAVE_EXPLORE: {thread th(&SlaveServerConnection::processExplore, this, unpackager.unpackageExploreRequest(), false);th.detach();} break;
			case PCKSLAVE_SELECT:  {thread th(&SlaveServerConnection::processSelect, this, unpackager.unpackageSelectRequest(), false);  th.detach();} break;
			case PCKSLAVE_ERROR:   processError();   break;
			default: break;
		}
	}

	void processStart(int slaveid);
	void processExist(string pk);
	vector<pair<int, string>>               processExplore(tuple<string, int, vector<string>> args, bool localinvoc);
	vector<pair<int, pair<string, string>>> processSelect(tuple<string, int, vector<string>> args, bool localinvoc);
	void processError();
};


void SlaveServerConnection::processStart(int myslaveid)
{
	cout << "processStart: Inicia" << endl;
	cout << "processStart: myslaveid = " << myslaveid << endl;
	slavesconn.myslaveid = myslaveid;
	cout << "processStart: Termina" << endl;
}

void SlaveServerConnection::processExist(string pk)
{
	cout << "processExist: Inicia" << endl;
	cout << "processExist: PK = " << pk << endl;
	string pack;
	if(database.existNode(pk)) pack = packager.packageTrue(); 
	else                       pack = packager.packageFalse();
	cout << "SlaveServerConnection: envio paquete = ["<<pack<<"]" << endl;
	sender.sendStr(pack);
}

// ejemplo: 2 03 C:/ 05 02 01 C 02 C:
// estructura: [cmd, len_pk, pk, prof, len_list, [len_pk, pk]+]
// longitudes: [1, 2 var, 2, 2 [2, var]+] 
vector<pair<int, string>> SlaveServerConnection::processExplore(tuple<string, int, vector<string>> args, bool localinvoc)
{
	cout << "SlaveServerConnection::processExplore: Inicia" << endl;
	string pk1; int prof; vector<string> visited;
	tie(pk1, prof, visited) = args;
	cout << "SlaveServerConnection::processExplore: pk1="<<pk1<<" prof="<<prof<<" lenlist="<<visited.size()<<endl;
	for(string vis: visited)
		cout << "SlaveServerConnection::processExplore(vis): vis=" << vis << endl;

	vector<string> new_visited;
	new_visited = concatVectors(new_visited, visited);
	new_visited.push_back(pk1);
	vector<pair<int, string>> explored;
	if(prof > 0){
		for(string pk2: database.getAllRelationships(pk1)){
			if(find(new_visited.begin(), new_visited.end(), pk2) != new_visited.end()) continue; // No visito los ya visitados
			vector<pair<int, string>> response;
			int slaveid = slavesconn.chooseSlave(pk2);
			cout << "SlaveServerConnection::processExplore(for): pk2="<<pk2<<" slaveid="<<slaveid<<endl;
			if(slaveid != slavesconn.myslaveid){
				string pack = packager.packageExploreRequest(pk2, prof-1, new_visited);
				cout << "SlaveServerConnection::processExplore(for): Envio pack["<<pack<<"]" << endl;
				slavesconn.sendPackToSlave(slaveid, pack);
				response = slavesconn.receivePackExplore(slaveid);
			} else {
				cout << "SlaveServerConnection::processExplore(for): local invocation"<<endl;
				response = processExplore(make_tuple(pk2, prof-1, new_visited), true);
			}
			for(pair<int,string> expl: response)
				cout << "SlaveServerConnection::processExplore(for)(expl): prof=" << expl.first << " pk=" << expl.second << endl;
			explored = concatVectors(explored, response);
		}
	}
	explored.push_back(make_pair(prof, pk1));
	cout << "SlaveServerConnection::processExplore: Termina" << endl;
	if(!localinvoc) {
		string pack = packager.packageExploreResponse(explored);
		cout << "SlaveServerConnection: envio paquete = ["<<pack<<"]" << endl;
		sender.sendStr(pack);
	}
	else 
		cout << "SlaveServerConnection::processExplore: retorno local invocation" << endl;
	return explored;
}

// casi igual que el explore
// ejemplo: 4 03 C:/ 05 02 01 C 02 C:
// estructura: [cmd, len_pk, pk, prof, len_list, [len_pk, pk]+]
// longitudes: [1, 2 var, 2, 2 [2, var]+] 
vector<pair<int, pair<string, string>>> SlaveServerConnection::processSelect(tuple<string, int, vector<string>> args, bool localinvoc)
{
	cout << "SlaveServerConnection::processSelect: Inicia" << endl;
	string pk1; int prof; vector<string> visited;
	tie(pk1, prof, visited) = args;
	cout << "SlaveServerConnection::processSelect: pk="<<pk1<<" prof="<<prof<<" lenlist="<<visited.size()<<endl;
	for(string vis: visited)
		cout << "SlaveServerConnection::processSelect(vis): vis=" << vis << endl;
	
	vector<string> new_visited;
	new_visited = concatVectors(new_visited, visited);
	new_visited.push_back(pk1);
	vector<pair<int, pair<string, string>>> explored;
	if(prof > 0){
		for(string pk2: database.getAllRelationships(pk1)){
			if(find(new_visited.begin(), new_visited.end(), pk2) != new_visited.end()) continue; // No visito los ya visitados
			vector<pair<int, pair<string, string>>> response;
			int slaveid = slavesconn.chooseSlave(pk2);
			cout << "SlaveServerConnection::processSelect(for): pk2="<<pk2<<" slaveid="<<slaveid<<endl;
			if(slaveid != slavesconn.myslaveid){
				string pack = packager.packageSelectRequest(pk2, prof-1, new_visited);
				cout << "SlaveServerConnection::processSelect(for): Envio pack["<<pack<<"]" << endl;
				slavesconn.sendPackToSlave(slaveid, pack);
				response = slavesconn.receivePackSelect(slaveid);
			} else {
				cout << "SlaveServerConnection::processSelect(for): local invocation"<<endl;
				response = processSelect(make_tuple(pk2, prof-1, new_visited), true);
			}
			for(pair<int, pair<string, string>> expl: response)
				cout << "SlaveServerConnection::processSelect(for)(expl): prof=" << expl.first << " pk=" << expl.second.first << "content="<<expl.second.second << endl;
			explored = concatVectors(explored, response);
		}
	}
	explored.push_back(make_pair(prof, make_pair(pk1, database.getContent(pk1))));
	cout << "SlaveServerConnection::processSelect: Termina" << endl;
	if(!localinvoc) {
		string pack = packager.packageSelectResponse(explored);
		cout << "SlaveServerConnection: envio paquete = ["<<pack<<"]" << endl;
		sender.sendStr(pack);
	}
	else 
		cout << "SlaveServerConnection::processSelect: retorno local invocation" << endl;
	return explored;
}

void SlaveServerConnection::processError()
{
	cout << "SlaveServerConnection: error paquete no reconocido" << endl;
	exit(0);
}


#endif