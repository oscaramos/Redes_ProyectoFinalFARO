#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H 
#include <iostream>
#include <vector>
#include <map>
#include <future>
#include <any>
#include "SlaveConnections.h"
#include "../peer/Client.h"
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
		char packid = receiver.recvChar();
		if(packid == '\0'){
			printf("Conexion con usuario cerrada\n");
			closeConnection();
			return;
		}
		cout << "packid = " << packid << endl;
		typeMasterPack typepack = verifierpack.getTypeOfPack(packid);
		switch(typepack)
		{
			case PCKMASTER_START:  {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageStart()); th.detach(); break;}
			case PCKMASTER_CREATE: {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageCreate()); th.detach(); break;}
			case PCKMASTER_LINK:   {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageLink()); th.detach(); break;}
			case PCKMASTER_DELETE: {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageDelete()); th.detach(); break;}			
			case PCKMASTER_UNLINK: {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageUnlink()); th.detach(); break;}
			case PCKMASTER_UPDATE: {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageUpdate()); th.detach(); break;}
			case PCKMASTER_EXPLORE:{thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageExplore()); th.detach(); break;}
			case PCKMASTER_SELECT: {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageSelect()); th.detach(); break;}
			case PCKMASTER_PING:   {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackagePing()); th.detach(); break;}
			case PCKMASTER_ERROR:  {thread th(&MasterConnection::responseMaster, this, typepack, unpackager.unpackageError()); th.detach(); break;}
		}
		cout << "receivePackagesHandler::Continuing_listening" << endl;
	}
		
private:
	void responseMaster(typeMasterPack typepack, any args)
	{
		future<string> futuremessage;
		switch(typepack)
		{
			case PCKMASTER_START:  futuremessage=async(&MasterConnection::processStart,this,  any_cast<vector<string>>(args));  break;
			case PCKMASTER_CREATE: futuremessage=async(&MasterConnection::processCreate, this, any_cast<tuple<string,string>>(args)); break;
			case PCKMASTER_LINK:   futuremessage=async(&MasterConnection::processLink, this, any_cast<tuple<string,string>>(args));   break;
			case PCKMASTER_DELETE: futuremessage=async(&MasterConnection::processDelete, this, any_cast<string>(args));break;
			case PCKMASTER_UNLINK: futuremessage=async(&MasterConnection::processUnlink, this, any_cast<tuple<string, string>>(args)); break;
			case PCKMASTER_UPDATE: futuremessage=async(&MasterConnection::processUpdate, this, any_cast<tuple<string, string>>(args)); break;
			case PCKMASTER_EXPLORE:futuremessage=async(&MasterConnection::processExplore, this, any_cast<tuple<string, int>>(args)); break;
			case PCKMASTER_SELECT: futuremessage=async(&MasterConnection::processSelect, this, any_cast<tuple<string, int>>(args)); break;
			case PCKMASTER_PING:   futuremessage=async(&MasterConnection::processPing, this); break;
			case PCKMASTER_ERROR:  futuremessage=async(&MasterConnection::processError, this); break;
		}
		string message = futuremessage.get();
		message = intWithZeros(message.size(), 3) + " " + message;
		cout << "Send master message = " << message << endl;
		sendMessageToMaster(message);
		cout << endl;
	}

	// Los casos de uso implementados
	string processStart(vector<string> ip_slaves);
	string processCreate(tuple<string, string> args);
	string processLink(tuple<string, string> args);
	string processDelete(string pk1);
	string processUnlink(tuple<string, string> args);
	string processUpdate(tuple<string, string> args);
	string processExplore(tuple<string, int> args);
	string processSelect(tuple<string, int> args);
	string processPing();
	string processError();

	// comunicaciones con Master y Slaves
	void sendMessageToMaster(string msg);

	// comunicaciones internas Slaves y Slaves
	void startSlaveid(int slaveid);
	void createNode(string pk, string content);
	void linkRelationship(string pk1, string pk2);
	void unlinkRelationship(string pk1, string pk2);
	bool existsNodeInSlaves(string pk);
	vector<pair<int,string>> exploreInSlaves(string pk, int prof);
	vector<pair<int,pair<string,string>>> selectInSlaves(string pk, int prof);

	// base de datos local
	bool existsNodeHere(string pk);
	bool existLinkHere(string pk1, string pk2);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


string MasterConnection::processStart(vector<string> ip_slaves)
{
	cout << "START: iniciado" << endl;
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

	for(unsigned int slaveid=0; slaveid<ip_slaves.size(); ++slaveid){
		startSlaveid(slaveid);
	}
	cout << "START: terminado" << endl;
	return "p"; 
}


string MasterConnection::processCreate(tuple<string, string> args)
{
	cout << "Create: iniciado" << endl;
	string pk, content; tie(pk, content) = args;
	cout << "Create: pk=" << pk << " content=" << content << endl;
	if(!existsNodeHere(pk)){
		database.createNode(pk, content);
		return "THE NODE WAS CREATED!";
	}
	else{
		return "THE NODE ALREADY EXISTS";
	}
}

string MasterConnection::processLink(tuple<string, string> args)
{
	cout << "Link: iniciado" << endl;
	string pk1, pk2; tie(pk1, pk2) = args;
	cout << "Link: pk1="<<pk1<<" pk2="<<pk2<< endl;
	if(existsNodeInSlaves(pk1) && existsNodeInSlaves(pk2)){
		bool isLinked = false;
		if(existsNodeHere(pk1)){
			if(!existLinkHere(pk1, pk2)){
				database.linkRelationship(pk1, pk2);
				isLinked = true;
			}
		}
		if(existsNodeHere(pk2)){
			if(!existLinkHere(pk2, pk1)){
				database.linkRelationship(pk2, pk1);
				isLinked = true;
			}
		}
		if(isLinked) return "THE NODES WERE LINKED!";
		else         return "THE LINK ALREADY EXISTS!";
	}
	else{
		return "SOME NODES DON’T EXIST";
	}
}

string MasterConnection::processDelete(string pk1)
{
	cout << "Delete: pk=" << pk1 << endl;
	if(existsNodeHere(pk1)){
		vector<string> relationships = database.getAllRelationships(pk1);
		for(string pk2: relationships){
			database.unlinkRelationship(pk1, pk2);
			unlinkRelationship(pk2, pk1);
		}
		database.deleteNode(pk1);
		return "THE NODE WAS DELETED!";
	}
	else
		return "THE NODE DON'T EXIST";
}


string MasterConnection::processUnlink(tuple<string, string> args)
{
	cout << "Unlink: iniciado" << endl;
	string pk1, pk2; tie(pk1, pk2) = args;
	cout << "Unlink: pk1="<<pk1<<" pk2="<<pk2<< endl;
	if(existsNodeInSlaves(pk1) && existsNodeInSlaves(pk2)){
		bool isUnlinked = false;
		if(existsNodeHere(pk1)){
			if(existLinkHere(pk1, pk2)){
				database.unlinkRelationship(pk1, pk2);
				isUnlinked = true;
			}
		}
		if(existsNodeHere(pk2)){
			if(existLinkHere(pk2, pk1)){
				database.unlinkRelationship(pk2, pk1);
				isUnlinked = true;
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


string MasterConnection::processUpdate(tuple<string, string> args)
{
	cout << "Update: iniciado" << endl;
	string old_pk, new_pk; tie(old_pk, new_pk) = args;
	cout << "Update: oldpk="<<old_pk<<" new_pk="<<new_pk<< endl;
	if(!existsNodeHere(old_pk))    return "THE OLD NODE DON'T EXIT";
	if(existsNodeInSlaves(new_pk)) return "THE NEW NODE ALREADY EXIST";
	createNode(new_pk, database.getContent(old_pk));
	vector<string> relationships = database.getAllRelationships(old_pk);
	for(string pk2: relationships){
		unlinkRelationship(pk2, old_pk);
		linkRelationship(pk2, new_pk);
		linkRelationship(new_pk, pk2);
	}
	database.deleteNode(old_pk);
	return "THE NODE WAS UPDATED!";
}


string MasterConnection::processExplore(tuple<string, int> args)
{
	cout << "MasterConnection::Explore: iniciado" << endl;
	string pk; int prof; tie(pk,  prof) = args;
	cout << "MasterConnection::Explore: pk="<<pk<<" prof="<<prof<<endl;
	string message;
	vector<pair<int,string>> explored = exploreInSlaves(pk, prof);
	reverse(explored.begin(), explored.end());
	for(pair<int,string> p: explored){
		cout << "MasterConnection::DATAExplore: rawprof="<<p.first<<" pk="<<p.second<<endl;
		message += string(2*(prof-p.first), '-') + ">" + p.second + "\n";
	}
	message.erase(message.end()-1);
	return message;
}


string MasterConnection::processSelect(tuple<string, int> args)
{
	cout << "MasterConnection::Select: iniciado" << endl;
	string pk; int prof; tie(pk,  prof) = args;
	cout << "MasterConnection::Select: pk="<<pk<<" prof="<<prof<<endl;
	string message;
	vector<pair<int,pair<string,string>>> explored = selectInSlaves(pk, prof); // retorna pair<prof, pk, content>
	reverse(explored.begin(), explored.end());
	for(pair<int,pair<string,string>> p: explored){
		cout << "MasterConnection::DATASelect: rawprof="<<p.first<<" pk="<<p.second.first<<" content="<<p.second.second<<endl;
		message += string(2*(prof-p.first), '-') + ">" + p.second.first + "\t("+p.second.second+")" + "\n";
	}
	message.erase(message.end()-1);
	return message;
}

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

void MasterConnection::startSlaveid(int slaveid)
{
	string pack = packager.packageStart(slaveid);
	cout << "startSlaveid: envio paquete ["<<pack<<"]" << endl;
	cout << "startSlaveid: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
}


void MasterConnection::createNode(string pk, string content)
{
	string pack = packager.packageCreate(pk, content);
	int slaveid = slavesconn.chooseSlave(pk);
	cout << "createNode: envio paquete ["<<pack<<"]" << endl;
	cout << "createNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	slaveid = slavesconn.nextSlaveid(slaveid);
	cout << "createNode: envio paquete ["<<pack<<"]" << endl;
	cout << "createNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
}

void MasterConnection::linkRelationship(string pk1, string pk2)
{
	string pack = packager.packageLink(pk1, pk2);
	int slaveid = slavesconn.chooseSlave(pk1);
	cout << "createNode: envio paquete ["<<pack<<"]" << endl;
	cout << "createNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	slaveid = slavesconn.nextSlaveid(slaveid);
	cout << "createNode: envio paquete ["<<pack<<"]" << endl;
	cout << "createNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
}

void MasterConnection::unlinkRelationship(string pk1, string pk2)
{
	string pack = packager.packageUnlink(pk1, pk2);
	int slaveid = slavesconn.chooseSlave(pk1);
	cout << "createNode: envio paquete ["<<pack<<"]" << endl;
	cout << "createNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
	slaveid = slavesconn.nextSlaveid(slaveid);
	cout << "createNode: envio paquete ["<<pack<<"]" << endl;
	cout << "createNode: al slave " << slaveid << endl;
	slavesconn.sendPackToSlave(slaveid, pack);
}

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