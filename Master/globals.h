#ifndef GLOBALS_H
#define GLOBALS_H
#include <vector>
#include "peer/Client.h"
#include "SlaveConnection.h"

#define DBG(x) cout << #x << " = " << x << endl;
#define DBGVEC(v) cout << #v << " = "; for(const auto&vv: v) cout << vv << ", "; cout << endl;

int k; // k = slaves.size();
std::vector< Client<SlaveConnection>* > slaves;

enum typeCmd{CMD_CONNECT, 
	         CMD_START, 
	         CMD_CREATE, 
	         CMD_LINK, 
	         CMD_DELETE, 
	         CMD_UNLINK, 
	         CMD_UPDATE, 
	         CMD_ERROR};

#endif