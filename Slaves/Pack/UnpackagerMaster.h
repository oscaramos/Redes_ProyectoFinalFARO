#ifndef UNPACKAGERMASTER_H
#define UNPACKAGERMASTER_H
#include <string>
#include <vector>
#include <tuple>
#include "../peer/ReceiverSocket.h"
#include "../aux/stringhelper.h"
using namespace std;

// atencion: usa receiver, puede afectar la recepcion de packetes
class UnpackagerMaster
{
public:
	ReceiverSocket receiver;

public:
	UnpackagerMaster(ReceiverSocket receiver): receiver(receiver)
	{
		;
	}

	// ejemplo: [0 02 08 10.0.0.1 08 10.0.0.2]
	// estructura: [1, 2 [2 var]+]
	// vector<ips>
	vector<string> unpackageStart()
	{
		vector<string> ips_slaves;
		receiver.ignore(1); int lenlist = receiver.recvInt(2);
		for(int i=0; i<lenlist; ++i){
			receiver.ignore(1); string ip = receiver.recvField(2);
			ips_slaves.push_back(ip);
		}
		return ips_slaves;
	}

	// ejemplo: [1 03 C:\ 018 size: 12, files: 2]
	// estructura: [1,2 var,3 var] 
	// <pk, content>
	tuple<string, string> unpackageCreate()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); string content = receiver.recvField(3);
		return make_tuple(pk, content);
	} 

	// ejemplo: [2 03 C:\ 12 C:\proyectos]
	// estructura: [1, 2 var, 2 var]
	// <pk1, pk2>
	tuple<string, string> unpackageLink()
	{
		receiver.ignore(1); string pk1 = receiver.recvField(2);
		receiver.ignore(1); string pk2 = receiver.recvField(2);
		return make_tuple(pk1, pk2);
	} 

	// ejemplo: [3 12 C:/proyectos]
	// <pk>
	string unpackageDelete()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		return pk;
	}

	// ejemplo: [4 03 C:\ 12 C:\proyectos]
	// estructura: [1, 2 var, 2 var]
	// <pk1, pk2>
	tuple<string, string> unpackageUnlink()
	{
		receiver.ignore(1); string pk1 = receiver.recvField(2);
		receiver.ignore(1); string pk2 = receiver.recvField(2);
		return make_tuple(pk1, pk2);
	} 

	// ejemplo: [5 03 C:\ 12 C:\proyectos]
	// estructura: [1, 2 var, 2 var]
	// <old_pk, new_pk> 
	tuple<string, string> unpackageUpdate()
	{
		receiver.ignore(1); string old_pk = receiver.recvField(2);
		receiver.ignore(1); string new_pk = receiver.recvField(2);
		return make_tuple(old_pk, new_pk);
	} 

	// ejemplo: [6 03 C:\ 05]
	// estructura: [1, 2 var, 2]
	// <pk, prof>
	tuple<string, int> unpackageExplore()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); int prof = receiver.recvInt(2);
		return make_tuple(pk, prof);
	}

	// ejemplo: [7 03 C:\ 05]
	// estructura: [1, 2 var, 2]
	// <pk, prof>
	tuple<string, int> unpackageSelect()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); int prof = receiver.recvInt(2);
		return make_tuple(pk, prof);
	}

	// ejemplo: [p]
	// estructura: [1]
	char unpackagePing()
	{
		return 'p'; // sin valor
	}

	char unpackageError()
	{
		return '?'; // sin valor
	}
};


#endif