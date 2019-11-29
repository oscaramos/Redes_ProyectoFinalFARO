#ifndef GLOBALS_H
#define GLOBALS_H


#include "Database.h"
#define DBG(x) cout << #x << " = " << x << endl;
#define DBGVEC(v) cout << #v << " = "; for(const auto&vv: v) cout << vv << ", "; cout << endl;

enum typeMasterPack{PCKMASTER_START, 
			        PCKMASTER_CREATE, 
			        PCKMASTER_LINK, 
			        PCKMASTER_DELETE, 
			        PCKMASTER_UNLINK, 
			        PCKMASTER_UPDATE, 
			        PCKMASTER_EXPLORE,
			        PCKMASTER_SELECT,
			        PCKMASTER_ERROR};

enum typeSlavePack{PCKSLAVE_EXIST,
				   PCKSLAVE_EXPLORE,
				   PCKSLAVE_SELECT,
				   PCKSLAVE_TRUE,
				   PCKSLAVE_FALSE,
                   PCKSLAVE_ERROR};

int port_master, port_slave;

Database database;

#endif