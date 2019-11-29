#ifndef AUXPACKCMD_H
#define AUXPACKCMD_H

#include <iostream>
#include "globals.h"
#include "aux/stringhelper.h"
using namespace std;

class VerifierMasterPack
{
public:	
	typeMasterPack getTypeOfPack(char firstchar)
	{
		switch(firstchar)
		{
			case '0': return PCKMASTER_START;
			case '1': return PCKMASTER_CREATE;
			case '2': return PCKMASTER_LINK;
			case '3': return PCKMASTER_DELETE;
			case '4': return PCKMASTER_UNLINK;
			case '5': return PCKMASTER_UPDATE;
			case '6': return PCKMASTER_EXPLORE;
		}
		return PCKMASTER_ERROR; 
	}
};


class VerifierSlavePack
{
public:	
	typeSlavePack getTypeOfPack(char firstchar)
	{
		switch(firstchar)
		{
			case '0': return PCKSLAVE_EXIST;
			case 'Y': return PCKSLAVE_TRUE;
			case 'N': return PCKSLAVE_FALSE;
		}
		return PCKSLAVE_ERROR; 
	}
};

class CreatorSlavePack
{
public:
	string createPack(typeSlavePack typepck)
	{
		string pack;
		switch(typepck)
		{
			case PCKSLAVE_TRUE: return "Y";
			case PCKSLAVE_FALSE: return "N";
			default: break;
		}
		return pack;
	}

	string createPack(typeSlavePack typepck, vector<string> args)
	{
		string pack;
		switch(typepck)
		{
			case PCKSLAVE_EXIST: pack = processExist(args); break;
			default: break;
		}
		return pack;
	}

private:
	string processExist(vector<string> args)
	{
		string pk = args[0];
		return join({"0", pkgF(pk, 2)});
	}

	// Solo para acortar codigo
	string pkgF(string field, int lendigits)
	{
		return packageField(field, lendigits);
	}
};


#endif