#ifndef SLAVECONNECTIONS_H
#define SLAVECONNECTIONS_H

#include <vector>
#include "SlaveClientConnection.h"

class SlaveConnections
{
public: // TODO: QUITAR PUBLIC
	int k; // k = slaves.size();
	vector< Client<SlaveClientConnection>* > slavesclients;
	vector<bool> isAlive; // TODO
	int myslaveid = -1; // refiere al slaveid de esta maquina

public:
	void sendPackToSlave(int slaveid, string pack)
	{	
		slavesclients[slaveid]->getInstanceOfPeerConnection()->sendPack(pack);
	}
	bool receivePackTrueOrFalseFromSlave(int slaveid)
	{
		return slavesclients[slaveid]->getInstanceOfPeerConnection()->receivePackTrueOrFalse();
	}

	vector<pair<int,string>> receivePackExplore(int slaveid)
	{
		return slavesclients[slaveid]->getInstanceOfPeerConnection()->receivePackExplore();
	}

	vector<pair<int, pair<string, string>>> receivePackSelect(int slaveid)
	{
		return slavesclients[slaveid]->getInstanceOfPeerConnection()->receivePackSelect();
	}

	// Puede presentar problemas!!, alerta, solo esta pensado para letras de la [a-z]?
	// fuente: https://cp-algorithms.com/string/string-hashing.html
	int chooseSlave(string pk)
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
};

#endif
