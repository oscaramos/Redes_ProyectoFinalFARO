#ifndef GLOBALS_H
#define GLOBALS_H

#define DBG(x) cout << #x << " = " << x << endl;
#define DBGVEC(v) cout << #v << " = "; for(const auto&vv: v) cout << vv << ", "; cout << endl;

enum typeCmd{CMD_CONNECT, 
	         CMD_START, 
	         CMD_CREATE, 
	         CMD_LINK, 
	         CMD_DELETE, 
	         CMD_UNLINK, 
	         CMD_UPDATE, 
	         CMD_EXPLORE,
	         CMD_SELECT,
	         CMD_ERROR};

#endif