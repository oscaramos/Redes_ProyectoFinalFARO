#ifndef AUXPACKCMD_H
#define AUXPACKCMD_H

#include <iostream>
#include "globals.h"
#include "aux/stringhelper.h"
using namespace std;

class VerifierCmd
{
public:	
	typeCmd getTypeOfCmd(string cmd)
	{
		string word = getFirstWord(cmd);
		word = capitalizeWord(word); // Para permitir "connect", ... 
		if(word == "CONNECT")	  return CMD_CONNECT;
		else if(word == "START")  return CMD_START;
		else if(word == "CREATE") return CMD_CREATE;
		else if(word == "LINK")   return CMD_LINK;
		else if(word == "DELETE") return CMD_DELETE;
		else if(word == "UNLINK") return CMD_UNLINK;
		else if(word == "UPDATE") return CMD_UPDATE;
		else return CMD_ERROR; 
	}

private:
	string getFirstWord(string cmd)
	{
		string s;
		for(size_t i=0; i<cmd.size() && isalpha(cmd[i]); ++i)
			s += cmd[i];
		return s;
	}

	string capitalizeWord(string word)
	{
		string capword;
		for(char c: word)
			capword += toupper(c);
		return capword;
	}
};

class CreatorPack
{
public:
	string createPack(typeCmd typecmd, string cmd)
	{
		string pack;
		vector<string> tokens = commandTokenizer(cmd);
		switch(typecmd)
		{
			case CMD_CREATE: pack = processCreate(tokens); break;
			case CMD_LINK:   pack = processLink(tokens);   break;
			case CMD_DELETE: pack = processDelete(tokens); break;
			case CMD_UNLINK: pack = processUnlink(tokens); break;
			case CMD_UPDATE: pack = processUpdate(tokens); break;
			default: break;
		}
		return pack;
	}

private:
	string processCreate(vector<string> tokens)
	{
		string pk = tokens[0];
	    string content = tokens[1];
	    return join({"1", pkgF(pk, 2), pkgF(content, 3)});
	}

	string processLink(vector<string> tokens)
	{
		string pk1 = tokens[0];
		string pk2 = tokens[1];
		return join({"2", pkgF(pk1, 2), pkgF(pk2, 2)});
	}

	string processDelete(vector<string> tokens)
	{
		string pk = tokens[0];
	    return join({"3", pkgF(pk, 2)});
	}

	string processUnlink(vector<string> tokens)
	{
		string pk1 = tokens[0];
		string pk2 = tokens[1];
		return join({"4", pkgF(pk1, 2), pkgF(pk2, 2)});
	}

	string processUpdate(vector<string> tokens)
	{
		string oldpk = tokens[0];
		string newpk = tokens[1];
		return join({"5", pkgF(oldpk, 2), pkgF(newpk, 2)});
	}

	// Solo para acortar codigo
	string pkgF(string field, int lendigits)
	{
		return packageField(field, lendigits);
	}
};

class MasterExtractorPK
{
public:
	string extractOnePk(typeCmd typecmd, string cmd)
	{
		string pk;
		vector<string> tokens = commandTokenizer(cmd);
		switch(typecmd)
		{
			case CMD_CREATE: pk = tokens[0]; break;
			case CMD_DELETE: pk = tokens[0]; break;
			case CMD_UPDATE: pk = tokens[0]; break;
			default: break; 
		}
		return pk;
	}

	pair<string, string> extractTwoPks(typeCmd typecmd, string cmd)
	{
		string pk1, pk2;
		vector<string> tokens = commandTokenizer(cmd);
		switch(typecmd)
		{
			case CMD_LINK:   pk1 = tokens[0]; pk2 = tokens[1]; break;
			case CMD_UNLINK: pk1 = tokens[0]; pk2 = tokens[1]; break;
			default: break;
		}
		return make_pair(pk1, pk2);
	}
};

#endif