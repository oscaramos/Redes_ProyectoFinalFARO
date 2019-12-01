#ifndef GLOBALS_H
#define GLOBALS_H
#include "Database.h"
#include "Connections/SlaveConnections.h"

enum typeMasterPack{PCKMASTER_START, 
			        PCKMASTER_CREATE, 
			        PCKMASTER_LINK, 
			        PCKMASTER_DELETE, 
			        PCKMASTER_UNLINK, 
			        PCKMASTER_UPDATE, 
			        PCKMASTER_EXPLORE,
			        PCKMASTER_SELECT,
			        PCKMASTER_PING,
			        PCKMASTER_ERROR};

enum typeSlavePack{PCKSLAVE_START,
				   PCKSLAVE_CREATE,
				   PCKSLAVE_LINK,
				   PCKSLAVE_UNLINK,
				   PCKSLAVE_EXIST,
				   PCKSLAVE_QEXPLORE, // Q = reQuest
				   PCKSLAVE_QSELECT,
				   PCKSLAVE_SEXPLORE, // S = reSponse
				   PCKSLAVE_SSELECT,
				   PCKSLAVE_TRUE,
				   PCKSLAVE_FALSE,
                   PCKSLAVE_ERROR};

int port_master, port_slave;

Database database;
SlaveConnections slavesconn;

template<typename T>
vector<T> concatVectors(vector<T> v1, vector<T> v2)
{
	vector<T> ans(v1.begin(), v1.end());
	ans.insert(ans.end(), v2.begin(), v2.end());
	return ans;
}

// debug
int slaveid = 0; 
bool debugMode = false;
//

#endif