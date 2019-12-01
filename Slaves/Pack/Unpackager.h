#ifndef UNPACKAGER_H
#define UNPACKAGER_H
#include <string>
#include <vector>
#include <tuple>
#include "../peer/ReceiverSocket.h"
#include "../aux/stringhelper.h"
using namespace std;


// atencion: usa receiver, puede afectar la recepcion de packetes
class Unpackager
{
public:
	ReceiverSocket receiver;

public:
	Unpackager(ReceiverSocket receiver): receiver(receiver)
	{
		;
	}

	int unpackageStart()
	{
		receiver.ignore(1);  int slaveid = receiver.recvInt(2);
		return slaveid;
	}

	tuple<string,string> unpackageCreate()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); string content = receiver.recvField(3);
		return make_tuple(pk, content);
	}

	tuple<string,string> unpackageLink()
	{
		receiver.ignore(1); string pk1 = receiver.recvField(2);
		receiver.ignore(1); string pk2 = receiver.recvField(2);
		return make_tuple(pk1, pk2);
	}

	tuple<string,string> unpackageUnlink()
	{
		receiver.ignore(1); string pk1 = receiver.recvField(2);
		receiver.ignore(1); string pk2 = receiver.recvField(2);
		return make_tuple(pk1, pk2);
	}


	string unpackageExist()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		return pk;
	}

	// <pk, prof, vector<pk>>
	tuple<string, int, vector<string>> unpackageExploreRequest()
	{
		receiver.ignore(1); string pk = receiver.recvField(2);
		receiver.ignore(1); int prof = receiver.recvInt(2);
		receiver.ignore(1); int len_list = receiver.recvInt(2);
		vector<string> visited;
		for(int i=0; i<len_list; ++i){
			receiver.ignore(1); string pk = receiver.recvField(2);
			visited.push_back(pk);
		}
		return make_tuple(pk, prof, visited);
	}

	// <pk, prof, vector<pk>>
	tuple<string, int, vector<string>> unpackageSelectRequest()
	{
		return unpackageExploreRequest();
	}

	vector<pair<int,string>> unpackageExploreResponse()
	{
		receiver.ignore(1); int len_list = receiver.recvInt(2);
		vector<pair<int,string>> explored;
		for(int i=0; i<len_list; ++i){
			receiver.ignore(1); int prof = receiver.recvInt(2);
			receiver.ignore(1); string pk = receiver.recvField(2);
			explored.push_back(make_pair(prof, pk));
		}
		return explored;
	}

	vector<pair<int,pair<string,string>>> unpackageSelectResponse()
	{
		receiver.ignore(1); int len_list = receiver.recvInt(2);
		vector<pair<int,pair<string,string>>> explored;
		for(int i=0; i<len_list; ++i){
			receiver.ignore(1); int prof = receiver.recvInt(2);
			receiver.ignore(1); string pk = receiver.recvField(2);
			receiver.ignore(1); string content = receiver.recvField(3);
			explored.push_back(make_pair(prof, make_pair(pk, content)));
		}
		return explored;
	}



};


#endif