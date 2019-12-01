#ifndef PACKAGER_H
#define PACKAGER_H
#include <string>
#include <vector>
#include "../aux/stringhelper.h"
#include "VerifierPacks.h"
using namespace std;


class Packager
{
private:
	VerifierSlavePack verifier;

public:
	// ejemplo: 0 01
	// estructura: [cmd, slaveid]
	// longitudes: [1, 2]
	string packageStart(int slaveid)
	{
		return join({verifier.getPackid(PCKSLAVE_START), intWithZeros(slaveid, 2)});
	}

	// ejemplo: 1 03 C:/ 004 hola
	// estructura: [cmd, len_pk, pk, len_content, content]
	// longitures: [1, 2 var, 3 var]
	string packageCreate(string pk, string content)
	{
		return join({verifier.getPackid(PCKSLAVE_CREATE), pkgF(pk, 2), pkgF(content, 3)});
	}

	// ejemplo: 2 03 C:/ 08 C:/a.exe
	// estructura: [cmd, len_pk1, pk1, len_pk2, pk2]
	// longitures: [1, 2 var]
	string packageLink(string pk1, string pk2)
	{
		return join({verifier.getPackid(PCKSLAVE_LINK), pkgF(pk1, 2), pkgF(pk2, 2)});
	}

	// ejemplo: 3 03 C:/ 08 C:/a.exe
	// estructura: [cmd, len_pk, pk]
	// longitures: [1, 2 var]
	string packageUnlink(string pk1, string pk2)
	{
		return join({verifier.getPackid(PCKSLAVE_UNLINK), pkgF(pk1, 2), pkgF(pk2, 2)});
	}

	// ejemplo: 4 03 C:/
	// estructura: [cmd, len_pk, pk]
	// longitudes: [1, 2 var]
	string packageExist(string pk)
	{
		return join({verifier.getPackid(PCKSLAVE_EXIST), pkgF(pk, 2)});
	}

	// ejemplo: 5 03 C:/ 05 02 01 C 02 C:
	// estructura: [cmd, len_pk pk, prof, len_list, [len_pk, pk]]
	// longitudes: [1, 2 var, 2, 2 [2 var]+] 
	string packageExploreRequest(string pk, int prof, vector<string> visited)
	{
		string pack = join({verifier.getPackid(PCKSLAVE_QEXPLORE), pkgF(pk, 2), intWithZeros(prof, 2), intWithZeros(visited.size(), 2)});
		for(string pk: visited)
			pack += " " + pkgF(pk, 2);
		return pack;
	}

	// lo mismo que el explore!! casi...
	// ejemplo: 6 03 C:/ 05 02 01 C 02 C:
	// estructura: [cmd, len_pk pk, prof, len_list, [len_pk, pk]]
	// longitudes: [1, 2 var, 2, 2 [2 var]+] 
	string packageSelectRequest(string pk, int prof, vector<string> visited)
	{
		string pack = packageExploreRequest(pk, prof, visited);
		pack[0] = verifier.getPackid(PCKSLAVE_QSELECT)[0];
		return pack;
	}

	// ejemplo: 7 01 00 03 C:/
	// estructura: [cmd, len_list, [prof, len_pk, pk]]
	// longitudes: [1, 2 [2, 2 var]+]
	string packageExploreResponse(vector<pair<int,string>> explored)
	{
		string pack = join({verifier.getPackid(PCKSLAVE_SEXPLORE), intWithZeros(explored.size(), 2)});
		for(auto p: explored){
			int prof = p.first; string pk = p.second;
			pack += " " + join({intWithZeros(prof, 2), pkgF(pk, 2)});
		}
		return pack;
	}

	// es como el explore, pero + contenido
	// ejemplo: 7 01 00 03 C:/ 24 (files: 2, size: 1044MB)
	// estructura: [cmd, len_list, [prof, len_pk, pk, len_content, content]]
	// longitudes: [1, 2 [2, 2 var, 3 var]+]
	string packageSelectResponse(vector<pair<int,pair<string,string> > > explored)
	{
		string pack = join({verifier.getPackid(PCKSLAVE_SSELECT), intWithZeros(explored.size(), 2)});
		for(auto p: explored){
			int prof = p.first; string pk = p.second.first; string content = p.second.second;
			pack += " " + join({intWithZeros(prof, 2), pkgF(pk, 2), pkgF(content, 3)});
		}
		return pack;
	}

	string packageTrue()
	{
		return verifier.getPackid(PCKSLAVE_TRUE);
	}

	string packageFalse()
	{
		return verifier.getPackid(PCKSLAVE_FALSE);
	}

private:
	// Solo para acortar codigo
	string pkgF(string field, int lendigits)
	{
		return packageField(field, lendigits);
	}
};


#endif