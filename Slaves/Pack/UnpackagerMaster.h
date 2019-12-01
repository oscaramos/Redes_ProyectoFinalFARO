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

	// <pk, content>
	tuple<string, string> unpackageCreate()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); string content = receiver.recvField(3);
		return make_tuple(pk, content);
	} 

	// <pk1, pk2>
	tuple<string, string> unpackageLink()
	{
		receiver.ignore(1); string pk1 = receiver.recvField(2);
		receiver.ignore(1); string pk2 = receiver.recvField(2);
		return make_tuple(pk1, pk2);
	} 

	// <pk>
	string unpackageDelete()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		return pk;
	}

	// <pk1, pk2>
	tuple<string, string> unpackageUnlink()
	{
		receiver.ignore(1); string pk1 = receiver.recvField(2);
		receiver.ignore(1); string pk2 = receiver.recvField(2);
		return make_tuple(pk1, pk2);
	} 

	// <pk, prof>
	tuple<string, int> unpackageExplore()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); int prof = receiver.recvInt(2);
		return make_tuple(pk, prof);
	}

	// <pk, prof>
	tuple<string, int> unpackageSelect()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); int prof = receiver.recvInt(2);
		return make_tuple(pk, prof);
	}
};


#endif