#ifndef CREATORPACKS_H
#define CREATORPACKS_H

#include <iostream>
#include "globals.h"
#include "aux/stringhelper.h"
using namespace std;

class CreatorSlavePack
{
public:
	string createPack(typeSlavePack typepck)
	{
		string pack;
		switch(typepck)
		{
			case PCKSLAVE_TRUE: return "T";
			case PCKSLAVE_FALSE: return "N";
			default: break;
		}
		return pack;
	}

	string createPackExist(typeSlavePack typepck, string pk)
	{
		return join({"1", pkgF(pk, 2)});
	}

	string createPackExploreRequest(string pk, int prof)
	{
		return createPackExploreRequest(pk, prof, vector<string>());
	}

	// ejemplo: 2 03 C:/ 05 02 01 C 02 C:
	// estructura: [cmd, len_pk pk, prof, len_list, [len_pk, pk]]
	// longitudes: [1, 2 var, 2, 2 [2 var]+] 
	string createPackExploreRequest(string pk, int prof, vector<string> visited)
	{
		string pack = join({"2", pkgF(pk, 2), intWithZeros(prof, 2), intWithZeros(visited.size(), 2)});
		for(string pk: visited)
			pack += " " + pkgF(pk, 2);
		return pack;
	}

	string createPackSelectRequest(string pk, int prof)
	{
		return createPackSelectRequest(pk, prof, vector<string>());
	}

	// lo mismo que el explore!! casi...
	// ejemplo: 3 03 C:/ 05 02 01 C 02 C:
	// estructura: [cmd, len_pk pk, prof, len_list, [len_pk, pk]]
	// longitudes: [1, 2 var, 2, 2 [2 var]+] 
	string createPackSelectRequest(string pk, int prof, vector<string> visited)
	{
		string pack = createPackExploreRequest(pk, prof, visited);
		pack[0] = '3';
		return pack;
	}

	// ejemplo: 4 01 00 03 C:/
	// estructura: [cmd, len_list, [prof, len_pk, pk]]
	// longitudes: [1, 2 [2, 2 var]+]
	string createPackExploreResponse(vector<pair<int,string> > explored)
	{
		string pack = join({"4", intWithZeros(explored.size(), 2)});
		for(auto p: explored){
			int prof = p.first; string pk = p.second;
			pack += " " + join({intWithZeros(prof, 2), pkgF(pk, 2)});
		}
		return pack;
	}

	// es como el explore, pero + contenido
	// ejemplo: 5 01 00 03 C:/ 24 (files: 2, size: 1044MB)
	// estructura: [cmd, len_list, [prof, len_pk, pk, len_content, content]]
	// longitudes: [1, 2 [2, 2 var, 3 var]+]
	string createPackSelectResponse(vector<pair<int,pair<string,string> > > explored)
	{
		string pack = join({"5", intWithZeros(explored.size(), 2)});
		for(auto p: explored){
			int prof = p.first; string pk = p.second.first; string content = p.second.second;
			pack += " " + join({intWithZeros(prof, 2), pkgF(pk, 2), pkgF(content, 3)});
		}
		return pack;
	}

private:
	// Solo para acortar codigo
	string pkgF(string field, int lendigits)
	{
		return packageField(field, lendigits);
	}
};


#endif