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
private:
	static const char id_start   = '0';
	static const char id_create  = '1';
	static const char id_link    = '2';
	static const char id_unlink  = '3';
	static const char id_exist   = '4';
	static const char id_qexplore = '5';
	static const char id_qselect  = '6';
	static const char id_sexplore = '7';
	static const char id_sselect  = '8';
	static const char id_true    = 'T';
	static const char id_false   = 'F';
	static const char id_error   = '?';

public:	
	typeSlavePack getTypeOfPack(char packid)
	{
		switch(packid)
		{
			case id_start:  return PCKSLAVE_START;
			case id_create: return PCKSLAVE_CREATE;
			case id_link:   return PCKSLAVE_LINK;
			case id_unlink: return PCKSLAVE_UNLINK;
			case id_exist:  return PCKSLAVE_EXIST;
			case id_qexplore:return PCKSLAVE_QEXPLORE;
			case id_qselect: return PCKSLAVE_QSELECT;
			case id_sexplore:return PCKSLAVE_SEXPLORE;
			case id_sselect: return PCKSLAVE_SSELECT;
			case id_true:   return PCKSLAVE_TRUE;
			case id_false:  return PCKSLAVE_FALSE;
			case id_error:  return PCKSLAVE_ERROR;
		}
		return PCKSLAVE_ERROR;
	}

	string getPackid(typeSlavePack typepack)
	{
		switch(typepack)
		{
			case PCKSLAVE_START:    return string(1, id_start);
			case PCKSLAVE_CREATE:   return string(1, id_create);
			case PCKSLAVE_LINK:     return string(1, id_link);
			case PCKSLAVE_UNLINK:   return string(1, id_unlink);
			case PCKSLAVE_EXIST:    return string(1, id_exist);
			case PCKSLAVE_QEXPLORE: return string(1, id_qexplore);
			case PCKSLAVE_QSELECT:  return string(1, id_qselect);
			case PCKSLAVE_SEXPLORE: return string(1, id_sexplore);
			case PCKSLAVE_SSELECT:  return string(1, id_sselect);
			case PCKSLAVE_TRUE:     return string(1, id_true);
			case PCKSLAVE_FALSE:    return string(1, id_false);
			case PCKSLAVE_ERROR:    return string(1, id_error);
		}
		return to_string(id_error);
	}
};

#endif