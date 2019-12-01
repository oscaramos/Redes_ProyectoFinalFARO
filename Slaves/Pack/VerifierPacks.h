#ifndef VERIFIERPACKS_H
#define VERIFIERPACKS_H

#include "../globals.h"

class VerifierMasterPack
{
public:	
	typeMasterPack getTypeOfPack(char packid)
	{
		switch(packid)
		{
			case '0': return PCKMASTER_START;
			case '1': return PCKMASTER_CREATE;
			case '2': return PCKMASTER_LINK;
			case '3': return PCKMASTER_DELETE;
			case '4': return PCKMASTER_UNLINK;
			case '5': return PCKMASTER_UPDATE;
			case '6': return PCKMASTER_EXPLORE;
			case '7': return PCKMASTER_SELECT;
			case 'p': return PCKMASTER_PING;
		}
		return PCKMASTER_ERROR; 
	}
};


class VerifierSlavePack
{
public:	
	typeSlavePack getTypeOfPack(char packid)
	{
		switch(packid)
		{
			case '0': return PCKSLAVE_START;
			case '1': return PCKSLAVE_EXIST;
			case '2': return PCKSLAVE_EXPLORE;
			case '3': return PCKSLAVE_SELECT;
			case 'T': return PCKSLAVE_TRUE;
			case 'N': return PCKSLAVE_FALSE;
		}
		return PCKSLAVE_ERROR; 
	}
};

#endif